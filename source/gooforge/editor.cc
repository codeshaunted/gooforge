// codeshaunted - gooforge
// source/gooforge/editor.cc
// contains Editor definitions
// Copyright 2024 codeshaunted
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org / licenses / LICENSE - 2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#include "editor.hh"

#include <variant>

#include "imgui.h"
#include "imgui-SFML.h"
#include "nfd.h"
#include "spdlog.h"
#include "glaze/json/read.hpp"
#include "glaze/json/write.hpp"

#include "constants.hh"
#include "resource_manager.hh"

namespace gooforge {

std::expected<void, Error> SelectEditorAction::execute(Editor* editor) {
    for (auto entity : this->entities) {
        entity->setSelected(true);
        editor->selected_entities.push_back(entity);
    }
    
    return std::expected<void, Error>{};
}

std::expected<void, Error> SelectEditorAction::revert(Editor* editor) {
    for (auto entity : this->entities) {
        entity->setSelected(false);

        // TODO: use an std::unordered_map so we don't have O(n) deletion
        for (size_t i = 0; i < editor->selected_entities.size(); ++i) {
            if (editor->selected_entities.at(i) == entity) {
                editor->selected_entities.erase(editor->selected_entities.begin() + i);
                break;
            }
        }
    }

    return std::expected<void, Error>{};
}

std::expected<void, Error> DeselectEditorAction::execute(Editor* editor) {
    for (auto entity : this->entities) {
        entity->setSelected(false);

        // TODO: use an std::unordered_map so we don't have O(n) deletion
        for (size_t i = 0; i < editor->selected_entities.size(); ++i) {
            if (editor->selected_entities.at(i) == entity) {
                editor->selected_entities.erase(editor->selected_entities.begin() + i);
                break;
            }
        }
    }

    return std::expected<void, Error>{};
}

std::expected<void, Error> DeselectEditorAction::revert(Editor* editor) {
    for (auto entity : this->entities) {
        entity->setSelected(true);
        editor->selected_entities.push_back(entity);
    }

    return std::expected<void, Error>{};
}

Editor::~Editor() {
    delete this->level;
}

void Editor::initialize() {
    this->window.create(sf::VideoMode(1920, 1080), "gooforge");
    this->window.setFramerateLimit(GOOFORGE_FRAMERATE_LIMIT);
    ImGui::SFML::Init(this->window);
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    this->view = sf::View(sf::FloatRect(0, 0, 1920, 1080));

    sf::Clock delta_clock;
    while (this->window.isOpen()) {
        this->update(delta_clock);
        this->draw();
    }

    ImGui::SFML::Shutdown();
}

void Editor::update(sf::Clock& delta_clock) {
    this->window.setView(this->view);

    this->processEvents();

    ImGui::SFML::Update(this->window, delta_clock.restart());

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    this->registerErrorDialog();
    this->registerSelectWOG2DirectoryDialog();
    this->registerMainMenuBar();
    this->registerLevelWindow();
    this->registerPropertiesWindow();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::O)) {
            this->doOpenFile();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z) && this->undo_clock.getElapsedTime() > this->undo_cooldown) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift)) {
                this->redoLastUndo();
            }
            else {
                this->undoLastAction();
            }

            this->undo_clock.restart();
        }
    }

    if (!std::filesystem::exists(this->wog2_path)) {
        this->showSelectWOG2DirectoryDialog();
    }

    if (!this->errors.empty()) {
        this->doCloseFile();
        this->showErrorDialog();
    }

    if (this->level) {
        this->level->update();
    }
}

void Editor::draw() {
    this->window.clear(sf::Color::White);
    if (this->level) {
        this->level->draw(&this->window);
    }
    ImGui::SFML::Render(this->window);
    this->window.display();
}

void Editor::processEvents() {
    sf::Event event;
    while (this->window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(this->window, event);

        if (event.type == sf::Event::Closed) {
            this->window.close();
        }

        if (event.type == sf::Event::Resized) {
            sf::FloatRect visible_area(0.f, 0.f, event.size.width, event.size.height);
            this->view = sf::View(visible_area);
        }

        if (ImGui::IsAnyItemHovered()) {
            continue;
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel) {
                if (event.mouseWheelScroll.delta > 0) {
                    this->view.zoom(0.9f);
                    this->zoom *= 0.9f;
                }
                else if (event.mouseWheelScroll.delta < 0) {
                    view.zoom(1.1f);
                    this->zoom *= 1.1f;
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Middle) {
                this->panning = true;
                this->pan_start_position = this->window.mapPixelToCoords(sf::Mouse::getPosition(this->window), this->view);
            }

            if (event.mouseButton.button == sf::Mouse::Left) {
                if (this->level) {
                    sf::Vector2f screen_click_position = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), this->view);
                    Vector2f world_click_position = Level::screenToWorld(screen_click_position);

                    this->level->sortEntities();
                    bool clicked_entity = false;
                    for (auto& entity : std::ranges::reverse_view(this->level->entities)) {
                        if (entity->wasClicked(world_click_position)) {
                            this->doEntitySelection(entity);
                            
                            clicked_entity = true;
                            break;
                        }
                    }

                    if (!clicked_entity && !this->selected_entities.empty()) {
                        this->doAction(DeselectEditorAction(this->selected_entities));
                    }
                }
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Middle) {
                this->panning = false;
            }
        }

        // Handle mouse movement for panning
        if (event.type == sf::Event::MouseMoved) {
            if (this->panning) {
                sf::Vector2f pan_position = this->window.mapPixelToCoords(sf::Mouse::getPosition(this->window), this->view);
                sf::Vector2f delta_pan = this->pan_start_position - pan_position;
                view.move(delta_pan);
            }
        }
    }
}

void Editor::doAction(EditorAction action) {
    this->undo_stack.push_front(action);
    BaseEditorAction* base_action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, action);

    // execute all implicit actions first
    for (EditorAction implicit_action : base_action->implicit_actions) {
        BaseEditorAction* base_implicit_action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, implicit_action);
        base_implicit_action->execute(this);
    }

    // execute the main action
    base_action->execute(this);
}

void Editor::undoAction(EditorAction action) {
    this->redo_stack.push_front(action);
    BaseEditorAction* base_action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, action);

    // revert the main (final) action first
    base_action->revert(this);

    // revert all implicit actions after
    for (EditorAction implicit_action : base_action->implicit_actions) {
        BaseEditorAction* base_implicit_action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, implicit_action);
        base_implicit_action->revert(this);
    }
}

void Editor::undoLastAction() {
    if (this->undo_stack.empty()) {
        return;
    }

    EditorAction last_action = this->undo_stack.front();
    this->undo_stack.pop_front();

    this->undoAction(last_action);
}

void Editor::redoLastUndo() {
    if (this->redo_stack.empty()) {
        return;
    }
}

void Editor::doEntitySelection(Entity* entity) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)) {
        if (entity->getSelected()) {
            this->doAction(DeselectEditorAction({ entity }));
        }
        else {
            this->doAction(SelectEditorAction({ entity }));
        }
    }
    else {
        this->doAction(SelectEditorAction({ entity }, { DeselectEditorAction(this->selected_entities) }));
    }
}

void Editor::doOpenFile() {
    NFD_Init();
    nfdu8char_t* out_path;
    nfdu8filteritem_t filters[2] = { {"World Of Goo 2", "wog2"} };
    nfdopendialogu8args_t args = { 0 };
    args.filterList = filters;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&out_path, &args);
    if (result == NFD_OKAY) {
        LevelInfo level_info;
        std::string buffer;
        auto level_info_error = glz::read_file_json < glz::opts{ .error_on_unknown_keys = false } > (level_info, out_path, buffer);

        if (level_info_error) {
            this->errors.push_back(JSONDeserializeError(out_path, glz::format_error(level_info_error, buffer)));
        }
        else {
            this->level_file_path = out_path;
            this->level = new Level();
            auto setup_result = this->level->setup(level_info);
            if (!setup_result) {
                this->errors.push_back(setup_result.error());
            }
        }

        NFD_FreePathU8(out_path);
    }
    NFD_Quit();
}

void Editor::doCloseFile() {
    delete this->level;
    this->level = nullptr;
    this->selected_entities.clear();
    this->undo_stack.clear();
    this->redo_stack.clear();
    ResourceManager::getInstance()->unloadAll();
}

void Editor::registerMainMenuBar() {
    float menu_bar_height = 0.0f;
    if (ImGui::BeginMainMenuBar()) {
        menu_bar_height = ImGui::GetWindowHeight();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                if (this->level) {
                    this->doCloseFile();
                }
                this->doOpenFile();
            }

            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                if (this->level) {
                    auto ec = glz::write_file_json < glz::opts{ .prettify = true } > (this->level->info, this->level_file_path, std::string{});
                }
            }

            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
                auto ec = glz::write_file_json<glz::opts{ .prettify = true }>(this->level->info, "./obj.json", std::string{});
            }

            if (ImGui::MenuItem("Close")) {
                this->doCloseFile();
            }

            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                window.close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::BeginDisabled(this->undo_stack.empty());
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                this->undoLastAction();
            }
            ImGui::EndDisabled();

            ImGui::BeginDisabled(this->redo_stack.empty());
            if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) {
                this->redoLastUndo();
            }
            ImGui::EndDisabled();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::registerErrorDialog() {
    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string message = "";
        if (!this->errors.empty()) {
            Error error_value = this->errors.back();
            BaseError* base_error = std::visit([](auto& derived_error) -> BaseError* { return &derived_error; }, error_value);
            message = base_error->getMessage();
        }

        ImGui::Text(message.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            this->errors.pop_back();
        }

        ImGui::EndPopup();
    }
}

void Editor::showErrorDialog() {
    ImGui::OpenPopup("Error");
}

void Editor::registerSelectWOG2DirectoryDialog() {
    if (ImGui::BeginPopupModal("Select World of Goo 2 Install", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Please select the 'game' directory in your World of Goo 2 install:");

        static char directory_path[512] = "";
        ImGui::InputText(" ", directory_path, IM_ARRAYSIZE(directory_path));
        ImGui::SameLine();
        if (ImGui::Button("...")) {
            NFD_Init();
            nfdu8char_t* out_path;
            nfdresult_t result = NFD_PickFolderU8(&out_path, NULL);
            if (result == NFD_OKAY) {
                std::strncpy(directory_path, out_path, 512);
                NFD_FreePathU8(out_path);
            }
            NFD_Quit();
        }

        ImGui::Separator();

        if (ImGui::Button("OK")) {
            this->wog2_path = std::filesystem::path(directory_path);
            ResourceManager::getInstance()->takeInventory(this->wog2_path);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Editor::showSelectWOG2DirectoryDialog() {
    ImGui::OpenPopup("Select World of Goo 2 Install");
}

void Editor::registerLevelWindow() {
    ImGui::Begin("Level");

    if (this->level) {
        size_t entity_i = 0;
        for (auto& entity : this->level->entities) {
            sf::Sprite sprite = entity->getThumbnail();
            std::string text = entity->getDisplayName();
            ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

            ImGui::PushID(static_cast<int>(entity_i));
            if (ImGui::Selectable("", entity->getSelected(), ImGuiSelectableFlags_SpanAllColumns)) {
                this->doEntitySelection(entity);
            }

            ImGui::SameLine();
            ImGui::Image(sprite, ImVec2(textSize.y, textSize.y));
            ImGui::SameLine();
            ImGui::Text("%s", text.c_str());

            ImGui::PopID();

            ++entity_i;
        }
    }

    ImGui::End();
}

void Editor::registerPropertiesWindow() {
    ImGui::Begin("Properties");

    if (this->level) {
        if (this->selected_entities.size() == 1) {
            Entity* entity = this->selected_entities[0];
            std::string text = entity->getDisplayName();
            ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

            ImGui::Image(entity->getThumbnail(), ImVec2(textSize.y * 2.0f, textSize.y * 2.0f));
            ImGui::SameLine();
            ImGui::Text(text.c_str());

            if (entity->getType() == EntityType::GOO_BALL) {
                GooBall* goo_ball = static_cast<GooBall*>(entity);
                GooBallInfo* info = goo_ball->getInfo();
                
                ImGui::SeparatorText("General Properties");
                if (ImGui::BeginTable("General Properties", 3, ImGuiTableFlags_SizingStretchProp)) {
                    // type
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Type");
                    ImGui::TableSetColumnIndex(2);
                    this->registerGooBallTypeCombo("", &info->typeEnum, goo_ball);

                    // position x
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Position");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("X");
                    ImGui::TableSetColumnIndex(2);
                    float pos_x = info->pos.x;
                    if (ImGui::InputFloat("##positionx", &pos_x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue)) {
                        this->doAction(ModifyPropertyEditorAction<float>(&info->pos.x, pos_x));
                    }

                    // position y
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("Y");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##positiony", &info->pos.y);

                    // rotation
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Rotation");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##rotation", &info->angle);

                    // max velocity
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Maximum Velocity");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##maxvelocity", &info->maxVelocity);

                    // stiffness
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Stiffness");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##stiffness", &info->stiffness);

                    // detonation radius
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Detonation Radius");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##detonationradius", &info->detonationRadius);

                    // detonation force
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Detonation Force");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##detonationforce", &info->detonationForce);

                    // discovered
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Discovered");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##discovered", &info->discovered);

                    // floating while asleep
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Float While Asleep");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##floatingwhileasleep", &info->floatingWhileAsleep);

                    // interactive
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Interactive");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##interactive", &info->interactive);

                    // wake with liquid
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Wake With Liquid");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##wakewithliquid", &info->wakeWithLiquid);

                    // exit pipe alert
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Exit Pipe Alert");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##exitpipealert", &info->exitPipeAlert);

                    // affects auto bounds
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Affects Auto Bounds");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##affectsautobounds", &info->affectsAutoBounds);

                    // filled
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Filled");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Checkbox("##filled", &info->filled);

                    ImGui::EndTable();
                }

                if (info->typeEnum == GooBallType::LAUNCHER_L2B || info->typeEnum == GooBallType::LAUNCHER_L2L) {
                    ImGui::SeparatorText("Launcher Properties");
                    if (ImGui::BeginTable("Launcher Properties", 3, ImGuiTableFlags_SizingStretchProp)) {
                        if (info->typeEnum == GooBallType::LAUNCHER_L2L) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Ball Type To Generate");
                            ImGui::TableSetColumnIndex(2);
                            this->registerGooBallTypeCombo("", &info->launcherBallTypeToGenerate);
                        }

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Minimum Lifespan");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputFloat("##launcherlifespanmin", &info->launcherLifespanMin);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Maximum Lifespan");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputFloat("##launcherlifespanmax", &info->launcherLifespanMax);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Knockback Factor");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputFloat("##knockbackfactor", &info->launcherKnockbackFactor);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Can Use Balls");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##launchercanuseballs", &info->launcherCanUseBalls);

                        ImGui::EndTable();
                    }
                }
                
                if (info->typeEnum == GooBallType::THRUSTER) {
                    ImGui::SeparatorText("Thruster Properties");
                    if (ImGui::BeginTable("Thruster Properties", 3, ImGuiTableFlags_SizingStretchProp)) {
                        ImGui::TableNextRow();
                        ImGui::SeparatorText("Thruster");
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Thrust Force");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputFloat("##thrustforce", &info->thrustForce);

                        ImGui::EndTable();
                    }
                }
            } else if (entity->getType() == EntityType::ITEM_INSTANCE) {
                ItemInstance* item_instance = static_cast<ItemInstance*>(entity);
                ItemInstanceInfo* info = item_instance->getInfo();

                if (ImGui::BeginTable("General Properties", 3, ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Depth");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::InputFloat("##depth", &info->depth);

                    ImGui::EndTable();
                }
            }
        }
    }

    ImGui::End();
}

// returns true if changed
bool Editor::registerGooBallTypeCombo(const char* label, GooBallType* type, GooBall* refresh_goo_ball) {
    bool changed = false;
    if (ImGui::BeginCombo(label, GooBall::ball_type_to_name.at(*type).c_str())) {
        for (auto& [name, id] : GooBall::ball_name_to_type) {
            if (name == "Invalid" || name == "LiquidLevelExit") {
                continue; // exclude these
            }

            bool selected = id == *type;
            if (ImGui::Selectable(name.c_str(), selected)) {
                this->doAction(ModifyPropertyEditorAction<GooBallType>(type, id, refresh_goo_ball));
                changed = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return changed;
}

} // namespace gooforge