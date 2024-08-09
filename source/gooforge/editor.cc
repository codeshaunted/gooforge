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
#include "simdjson.h"
#include "spdlog.h"
#include "glaze/json/read.hpp"

#include "constants.hh"
#include "resource_manager.hh"

namespace gooforge {

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

void Editor::update(sf::Clock delta_clock) {
    this->window.setView(this->view);

    this->processEvents();

    ImGui::SFML::Update(this->window, delta_clock.restart());

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    this->registerErrorDialog();
    this->registerSelectWOG2DirectoryDialog();
    this->registerMainMenuBar();
    this->registerLevelWindow();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::O)) {

        }
    }

    if (!std::filesystem::exists(this->wog2_path)) {
        this->showSelectWOG2DirectoryDialog();
    }

    if (this->error) {
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
                    for (auto entity : std::ranges::reverse_view(this->level->entities)) {
                        if (entity->wasClicked(world_click_position)) {
                            this->doEntitySelection(entity);
                            
                            clicked_entity = true;
                            break;
                        }
                    }

                    if (!clicked_entity) {
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

void Editor::doAction(EditorAction action, bool for_redo) {
    this->undo_stack.push_front(action);
    BaseEditorAction* base_action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, action);
    if (!base_action->implicit && !for_redo) {
        ++this->redo_stack_count;
    }
    if (base_action->implicit && !for_redo) {
        this->redo_stack_count = 0;
        this->redo_stack.clear();
    }

    if (SelectEditorAction* select_action = std::get_if<SelectEditorAction>(&action)) {
        for (auto entity : select_action->entities) {
            entity->setSelected(true);
            this->selected_entities.push_back(entity);
        }
    } else if (DeselectEditorAction* deselect_action = std::get_if<DeselectEditorAction>(&action)) {
        for (auto entity : deselect_action->entities) {
            entity->setSelected(false);
            
            // TODO: use an std::unordered_map so we don't have O(n) deletion
            for (size_t i = 0; i < this->selected_entities.size(); ++i) {
                if (this->selected_entities.at(i) == entity) {
                    this->selected_entities.erase(this->selected_entities.begin() + i);
                    break;
                }
            }
        }
    }
}

void Editor::undoAction(EditorAction action) {
    if (SelectEditorAction* select_action = std::get_if<SelectEditorAction>(&action)) {
        for (auto entity : select_action->entities) {
            entity->setSelected(false);

            // TODO: use an std::unordered_map so we don't have O(n) deletion
            for (size_t i = 0; i < this->selected_entities.size(); ++i) {
                if (this->selected_entities.at(i) == entity) {
                    this->selected_entities.erase(this->selected_entities.begin() + i);
                    break;
                }
            }
        }
    }
    else if (DeselectEditorAction* deselect_action = std::get_if<DeselectEditorAction>(&action)) {
        for (auto entity : deselect_action->entities) {
            entity->setSelected(true);
            this->selected_entities.push_back(entity);
        }

    }
}

void Editor::undoLastAction() {
    EditorAction last_action = this->undo_stack.front();
    this->undo_stack.pop_front();
    this->redo_stack.push_front(last_action);
    ++this->redo_stack_count;
    this->undoAction(last_action);

    if (this->undo_stack.empty()) {
        return;
    }

    // thank you modern c++ for the following abomination
    // all you need to know is that this also undoes all implicit actions and adds them to the redo stack
    last_action = this->undo_stack.front();
    for (BaseEditorAction* action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, last_action); action->implicit;) {
        this->undo_stack.pop_front();
        this->redo_stack.push_front(last_action);
        this->undoAction(last_action);

        if (this->undo_stack.empty()) {
            break;
        }

        last_action = this->undo_stack.front();
    }

    --this->undo_stack_count;
}

void Editor::redoLastUndo() {
    EditorAction last_action = this->redo_stack.back();
    for (BaseEditorAction* action = std::visit([](auto& derived_action) -> BaseEditorAction* { return &derived_action; }, last_action); action->implicit;) {
        this->redo_stack.pop_back();
        this->doAction(last_action);

        if (this->redo_stack.empty()) {
            break;
        }

        last_action = this->redo_stack.back();
    }

    this->doAction(last_action);

    this->redo_stack.pop_back();
    --this->redo_stack_count;
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
        this->doAction(DeselectEditorAction(this->selected_entities, true));
        this->doAction(SelectEditorAction({ entity }));
    }
}

void Editor::registerMainMenuBar() {
    float menu_bar_height = 0.0f;
    if (ImGui::BeginMainMenuBar()) {
        menu_bar_height = ImGui::GetWindowHeight();
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
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
                    auto level_info_error = glz::read_file_json<glz::opts{ .error_on_unknown_keys = false }>(level_info, out_path, buffer);

                    if (level_info_error) {
                        this->error = JSONDeserializeError(out_path, glz::format_error(level_info_error, buffer));
                    }
                    else {
                        this->level = new Level(level_info);
                    }

                    NFD_FreePathU8(out_path);
                }
                NFD_Quit();
            }



            if (ImGui::MenuItem("Save", "Ctrl+S")) {

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
        if (this->error) {
            Error error_value = this->error.value();
            BaseError* base_error = std::visit([](auto& derived_error) -> BaseError* { return &derived_error; }, error_value);
            message = base_error->getMessage();
        }

        ImGui::Text(message.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            this->error = std::nullopt;
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
            ResourceManager::getInstance()->loadManifest((this->wog2_path / "res/balls/_atlas.image.atlas").generic_string());
            GooBall::loadGooBallTemplates((this->wog2_path / "res/balls/").generic_string()); // calling this here is a little hacky and coupling, TODO: fix?
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
            if (entity->getType() == EntityType::GOO_BALL) {
                GooBall* goo_ball = static_cast<GooBall*>(entity);
                sf::Sprite sprite = *(*ResourceManager::getInstance()->getSpriteResource(goo_ball->getTemplate()->body_image_id))->get();
                const char* text = "Goo Ball";
                ImVec2 textSize = ImGui::CalcTextSize(text);

                // Draw selectable item with custom layout
                ImGui::PushID(static_cast<int>(entity_i));
                if (ImGui::Selectable("", entity->getSelected(), ImGuiSelectableFlags_SpanAllColumns)) {
                    this->doEntitySelection(entity);
                }

                ImGui::SameLine();
                ImGui::Image(sprite, ImVec2(textSize.y, textSize.y)); // Draw image
                ImGui::SameLine();
                ImGui::Text("%s", text); // Draw text

                ImGui::PopID();
            }

            ++entity_i;
        }
    }

    ImGui::End();
}

} // namespace gooforge