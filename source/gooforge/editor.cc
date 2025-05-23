// codeshaunted - gooforge
// source/gooforge/editor.cc
// contains Editor definitions
// Copyright (C) 2024 codeshaunted
//
// This file is part of gooforge.
// gooforge is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// gooforge is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gooforge. If not, see <https://www.gnu.org/licenses/>.

#include "editor.hh"

#include <format>
#include <variant>

#include "glaze/json/read.hpp"
#include "glaze/json/write.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "nfd.h"
#include "spdlog.h"

#include "constants.hh"
#include "resource_manager.hh"

namespace gooforge {

EditorAction::~EditorAction() {
    for (auto action : this->implicit_actions) {
        delete action;
    }
}

std::expected<void, Error> SwitchToolEditorAction::execute(Editor* editor) {
    this->old_tool = editor->selected_tool;
    editor->selected_tool = this->tool;

    return std::expected<void, Error>{};
}

std::expected<void, Error> SwitchToolEditorAction::revert(Editor* editor) {
    editor->selected_tool = this->old_tool;

    return std::expected<void, Error>{};
}

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
                editor->selected_entities.erase(
                    editor->selected_entities.begin() + i);
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
                editor->selected_entities.erase(
                    editor->selected_entities.begin() + i);
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

CreateEditorAction::~CreateEditorAction() {
    if (!reverted) return; // if the creation was reverted, delete the entity

    delete this->entity;
}

std::expected<void, Error> CreateEditorAction::execute(Editor* editor) {
    this->reverted = false;

    editor->level->addEntity(this->entity);

    return std::expected<void, Error>{};
}

std::expected<void, Error> CreateEditorAction::revert(Editor* editor) {
    this->reverted = true;

    editor->level->removeEntity(this->entity);

    return std::expected<void, Error>{};
}

DeleteEditorAction::~DeleteEditorAction() {
    if (reverted)
        return; // we don't want to run this if the delete was reverted and is
                // getting cleared in the redo stack

    // THIS IS WHERE ALL ENTITIES WILL BE ACTUALLY DELETED
    // we keep them around in here so that undo/redo works
    for (auto entity : this->entities) {
        delete entity;
    }
}

std::expected<void, Error> DeleteEditorAction::execute(Editor* editor) {
    this->reverted = false;

    for (auto entity : this->entities) {
        editor->level->removeEntity(entity);
    }

    return std::expected<void, Error>{};
}

std::expected<void, Error> DeleteEditorAction::revert(Editor* editor) {
    this->reverted = true;

    for (auto entity : std::ranges::reverse_view(this->entities)) {
        editor->level->addEntity(entity);
    }

    return std::expected<void, Error>{};
}

template <>
void Editor::registerPropertiesField(
    const char* label, std::function<Vector2f()> get,
    std::function<void(Vector2f)> set,
    std::vector<EditorAction*> implicit_actions) {
    Vector2f value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("X");
    ImGui::TableSetColumnIndex(2);
    modified |=
        ImGui::InputFloat(std::format("##{}x", label).c_str(), &value.x, 0.0f,
                          0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("Y");
    ImGui::TableSetColumnIndex(2);
    modified |=
        ImGui::InputFloat(std::format("##{}y", label).c_str(), &value.y, 0.0f,
                          0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<Vector2f>(
            get, set, value, implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField(
    const char* label, std::function<GooBallType()> get,
    std::function<void(GooBallType)> set,
    std::vector<EditorAction*> implicit_actions) {
    GooBallType value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);
    if (ImGui::BeginCombo("", GooBall::ball_type_to_name.at(value).c_str())) {
        for (auto& [name, id] : GooBall::ball_name_to_type) {
            if (name == "Invalid" || name == "LiquidLevelExit") {
                continue; // exclude these
            }

            bool selected = id == value;
            if (ImGui::Selectable(name.c_str(), selected)) {
                value = id;
                modified = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<GooBallType>(
            get, set, value, implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField(
    const char* label, std::function<float()> get,
    std::function<void(float)> set,
    std::vector<EditorAction*> implicit_actions) {
    float value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);
    modified |=
        ImGui::InputFloat(std::format("##{}", label).c_str(), &value, 0.0f,
                          0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<float>(get, set, value,
                                                             implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField(
    const char* label, std::function<int()> get, std::function<void(int)> set,
    std::vector<EditorAction*> implicit_actions) {
    int value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);
    modified |= ImGui::InputInt(std::format("##{}", label).c_str(), &value, 0,
                                0, ImGuiInputTextFlags_EnterReturnsTrue);

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<int>(get, set, value,
                                                           implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField(
    const char* label, std::function<bool()> get, std::function<void(bool)> set,
    std::vector<EditorAction*> implicit_actions) {
    bool value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);
    modified |= ImGui::Checkbox(std::format("##{}", label).c_str(), &value);

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<bool>(get, set, value,
                                                            implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField<std::string, ItemTemplatePropertyTag>(
    const char* label, std::function<std::string()> get,
    std::function<void(std::string)> set,
    std::vector<EditorAction*> implicit_actions) {
    std::string value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);

    // calling this every frame is not ideal
    // but premature optimization is the root of all evil or whatever that guy
    // said
    // ;)
    auto value_resource =
        ResourceManager::getInstance()->getResource<ItemResource>(
            std::format("GOOFORGE_ITEM_RESOURCE_{}", value));
    if (!value_resource) {
        return; // todo: actually handle this error
    }
    auto value_result = value_resource.value()->get();
    if (!value_result) {
        return; // todo: actually handle this error
    }
    ItemInfo& item_info = value_result.value()->items[0]; // not safe

    if (ImGui::BeginCombo(
            std::format("##{}", label).c_str(),
            std::format("{} ({})", item_info.name, value).c_str())) {
        static char filter[128] = "";
        ImGui::InputText(std::format("##{}itemfilter", label).c_str(), filter,
                         IM_ARRAYSIZE(filter));
        ImGui::Separator();

        auto item_resources =
            ResourceManager::getInstance()->getResources<ItemResource>(filter,
                                                                       25);

        if (!item_resources) {
            ImGui::EndCombo();
            return; // todo: actually handle this error
        }

        for (auto item_resource : *item_resources) {
            auto item_result = item_resource->get();
            if (!item_result) {
                continue;
            }

            ItemInfo& item_info = item_result.value()->items[0]; // NOT SAFE
            bool selected = item_info.uuid == value;
            if (ImGui::Selectable(
                    std::format("{} ({})", item_info.name, item_info.uuid)
                        .c_str(),
                    selected)) {
                value = item_info.uuid;
                modified = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<std::string>(
            get, set, value, implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField<std::string, TerrainTemplatePropertyTag>(
    const char* label, std::function<std::string()> get,
    std::function<void(std::string)> set,
    std::vector<EditorAction*> implicit_actions) {
    std::string value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);

    auto terrains_resource =
        ResourceManager::getInstance()->getResource<TerrainTemplatesResource>(
            "GOOFORGE_TERRAIN_TEMPLATES_RESOURCE");
    if (!terrains_resource) {
        return; // todo: actually handle this error
    }

    auto terrains_result = (*terrains_resource)->get();
    if (!terrains_result) {
        return; // todo: actually handle this error
    }

    auto terrains = *terrains_result;

    // this is horribly inefficient, todo: fix
    std::string selected_name = "";

    for (auto terrain : terrains->terrainTypes) {
        if (terrain.uuid == value) {
            selected_name = terrain.name;
            break;
        }
    }

    if (ImGui::BeginCombo(
            std::format("##{}", label).c_str(),
            std::format("{} ({})", selected_name, value).c_str())) {
        for (auto terrain : terrains->terrainTypes) {
            bool selected = terrain.uuid == value;
            if (ImGui::Selectable(
                    std::format("{} ({})", terrain.name, terrain.uuid).c_str(),
                    selected)) {
                value = terrain.uuid;
                modified = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<std::string>(
            get, set, value, implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField<TerrainGroup*>(
    const char* label, std::function<TerrainGroup*()> get,
    std::function<void(TerrainGroup*)> set,
    std::vector<EditorAction*> implicit_actions) {
    TerrainGroup* value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);

    if (ImGui::BeginCombo(std::format("##{}", label).c_str(),
                          value ? value->getDisplayName().c_str() : "None")) {
        if (ImGui::Selectable("None", value == nullptr)) {
            value = nullptr;
            modified = true;
        }

        for (auto entity : this->level->entities) {
            if (entity->getType() != EntityType::TERRAIN_GROUP) {
                continue;
            }

            TerrainGroup* terrain_group = static_cast<TerrainGroup*>(entity);

            bool selected = terrain_group == value;
            if (ImGui::Selectable(terrain_group->getDisplayName().c_str(),
                                  selected)) {
                value = terrain_group;
                modified = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<TerrainGroup*>(
            get, set, value, implicit_actions));
    }
}

template <>
void Editor::registerPropertiesField<LiquidType>(
    const char* label, std::function<LiquidType()> get,
    std::function<void(LiquidType)> set,
    std::vector<EditorAction*> implicit_actions) {
    LiquidType value = get();
    bool modified = false;

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(2);

    if (ImGui::BeginCombo(std::format("##{}", label).c_str(),
                          liquid_type_to_name[value].c_str())) {
        for (auto type : liquid_type_to_name) {
            bool selected = type.first == value;
            if (ImGui::Selectable(liquid_type_to_name[type.first].c_str(),
                                  selected)) {
                value = type.first;
                modified = true;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (modified) {
        this->doAction(new ModifyPropertyEditorAction<LiquidType>(
            get, set, value, implicit_actions));
    }
}

Editor::~Editor() { delete this->level; }

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

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    this->registerErrorDialog();
    this->registerSelectWOG2DirectoryDialog();
    this->registerMainMenuBar();
    this->registerLevelWindow();
    this->registerPropertiesWindow();
    this->registerResourcesWindow();
    this->registerToolbarWindow();

    if (this->selected_tool == EditorToolType::STRAND) {
        if (this->selected_entities.size() == 1 &&
            this->selected_entities[0]->getType() == EntityType::GOO_BALL) {
            this->strand_start_ball =
                static_cast<GooBall*>(this->selected_entities[0]);
        } else if (this->strand_start_ball &&
                   this->selected_entities.size() == 2 &&
                   this->selected_entities[0]->getType() ==
                       EntityType::GOO_BALL &&
                   this->selected_entities[1]->getType() ==
                       EntityType::GOO_BALL) {
            GooStrandInfo strand_info;
            strand_info.type = this->strand_start_ball->getBallType();
            GooStrand* strand = new GooStrand();
            strand->setup(strand_info,
                          static_cast<GooBall*>(this->selected_entities[0]),
                          static_cast<GooBall*>(
                              this->selected_entities[1])); // ignore errors for
                                                            // now, todo: handle

            // we are faux-selecting both of them when we create the strand here
            // very cursed, should be replaced at some point
            for (auto entity : this->selected_entities) {
                entity->setSelected(false);
            }
            this->selected_entities.clear();

            this->doAction(new CreateEditorAction(
                strand, {new DeselectEditorAction({this->strand_start_ball})}));

            this->strand_start_ball = nullptr;
        } else {
            this->strand_start_ball = nullptr;
        }
    }

    // this is horrid, todo: fix
    static bool dragging = false;
    sf::Vector2i mouse_pos = sf::Mouse::getPosition();
    static sf::Vector2i drag_start;
    static std::unordered_map<Entity*, Vector2f> pre_drag_positions;
    ImGuiIO& io = ImGui::GetIO();
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !io.WantCaptureMouse &&
        !io.AppFocusLost) {
        if (!dragging) {
            dragging = true;
            drag_start = mouse_pos;

            for (Entity* entity : this->selected_entities) {
                pre_drag_positions.insert({entity, entity->getPosition()});
            }
        } else {
            sf::Vector2i drag_delta = drag_start - mouse_pos;
            drag_start = mouse_pos;

            if (this->selected_tool == EditorToolType::MOVE) {
                Vector2f world_drag_delta =
                    Level::screenToWorld(sf::Vector2f(drag_delta)) * this->zoom;

                for (Entity* entity : this->selected_entities) {
                    entity->setPosition(entity->getPosition() -
                                        world_drag_delta);
                }
            }
        }
    } else if (dragging) {
        dragging = false;

        sf::Vector2i drag_delta = drag_start - mouse_pos;
        drag_start = mouse_pos;

        if (this->selected_tool == EditorToolType::MOVE) {
            Vector2f world_drag_delta =
                Level::screenToWorld(sf::Vector2f(drag_delta)) * this->zoom;

            for (auto entity : this->selected_entities) {
                Vector2f pre_drag_position = pre_drag_positions[entity];

                this->doAction(new ModifyPropertyEditorAction<Vector2f>(
                    [pre_drag_position]() { return pre_drag_position; },
                    [entity](Vector2f value) { entity->setPosition(value); },
                    entity->getPosition() - world_drag_delta, {}));
            }
        }

        pre_drag_positions.clear();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::O)) {
            this->doOpenFile();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Z) &&
            this->undo_clock.getElapsedTime() > this->undo_cooldown) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift)) {
                this->redoLastUndo();
            } else {
                this->undoLastAction();
            }

            this->undo_clock.restart();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Delete) &&
        !this->selected_entities.empty()) {
        this->doEntitiesDeletion(this->selected_entities);
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

    // todo: move this abomination elsewhere
    if (this->selected_tool == EditorToolType::STRAND &&
        this->strand_start_ball) {
        auto mouse_pos = sf::Mouse::getPosition(this->window);
        auto ball_pos =
            Level::worldToScreen(this->strand_start_ball->getPosition());

        sf::Vertex line[] = {
            sf::Vertex(ball_pos, sf::Color::Blue),
            sf::Vertex(this->window.mapPixelToCoords(mouse_pos),
                       sf::Color::Blue),
        };

        window.draw(line, 2, sf::Lines);
    }

    ImGui::SFML::Render(this->window);
    this->window.display();
}

void Editor::processEvents() {
    sf::Event event;
    while (this->window.pollEvent(event)) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SFML::ProcessEvent(this->window, event);

        if (event.type == sf::Event::Closed) {
            this->window.close();
        }

        if (event.type == sf::Event::Resized) {
            sf::FloatRect visible_area(0.f, 0.f, event.size.width,
                                       event.size.height);
            this->view = sf::View(visible_area);
        }

        if (io.WantCaptureMouse || io.AppFocusLost) {
            continue;
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel ==
                sf::Mouse::Wheel::VerticalWheel) {
                if (event.mouseWheelScroll.delta > 0) {
                    this->view.zoom(0.9f);
                    this->zoom *= 0.9f;
                } else if (event.mouseWheelScroll.delta < 0) {
                    view.zoom(1.1f);
                    this->zoom *= 1.1f;
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Middle) {
                this->panning = true;
                this->pan_start_position = this->window.mapPixelToCoords(
                    sf::Mouse::getPosition(this->window), this->view);
            }

            if (event.mouseButton.button == sf::Mouse::Left) {
                if (this->level) {
                    sf::Vector2f screen_click_position =
                        window.mapPixelToCoords(
                            sf::Vector2i(event.mouseButton.x,
                                         event.mouseButton.y),
                            this->view);
                    Vector2f world_click_position =
                        Level::screenToWorld(screen_click_position);

                    bool clicked_entity = false;
                    for (auto entity :
                         std::ranges::reverse_view(this->level->entities)) {
                        if (entity->wasClicked(world_click_position)) {
                            this->doEntitySelection(entity);

                            clicked_entity = true;
                            break;
                        }
                    }

                    if (!clicked_entity && !this->selected_entities.empty()) {
                        this->doAction(
                            new DeselectEditorAction(this->selected_entities));
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
                sf::Vector2f pan_position = this->window.mapPixelToCoords(
                    sf::Mouse::getPosition(this->window), this->view);
                sf::Vector2f delta_pan =
                    this->pan_start_position - pan_position;
                view.move(delta_pan);
            }
        }
    }
}

void Editor::doAction(EditorAction* action) {
    this->clearRedos();

    if (this->undo_stack.size() == this->undo_depth) {
        delete this->undo_stack.back();
        this->undo_stack.pop_back();
    }
    this->undo_stack.push_front(action);

    // execute all implicit actions first
    for (auto implicit_action : action->implicit_actions) {
        implicit_action->execute(this);
    }

    // execute the main action
    action->execute(this);
}

void Editor::undoAction(EditorAction* action) {
    this->redo_stack.push_front(action);

    // revert the main (final) action first
    action->revert(this);

    // revert all implicit actions after
    for (auto implicit_action : action->implicit_actions) {
        implicit_action->revert(this);
    }
}

// same as doAction but we don't clear redos
void Editor::redoAction(EditorAction* action) {
    if (this->undo_stack.size() == this->undo_depth) {
        delete this->undo_stack.back();
        this->undo_stack.pop_back();
    }
    this->undo_stack.push_front(action);

    // execute all implicit actions first
    for (auto implicit_action : action->implicit_actions) {
        implicit_action->execute(this);
    }

    // execute the main action
    action->execute(this);
}

void Editor::undoLastAction() {
    if (this->undo_stack.empty()) {
        return;
    }

    auto last_action = this->undo_stack.front();
    this->undo_stack.pop_front();

    this->undoAction(last_action);
}

void Editor::redoLastUndo() {
    if (this->redo_stack.empty()) {
        return;
    }

    auto last_undo = this->redo_stack.front();
    this->redo_stack.pop_front();

    this->redoAction(last_undo);
}

void Editor::clearUndos() {
    for (auto undo : this->undo_stack) {
        delete undo;
    }

    this->undo_stack.clear();
}

void Editor::clearRedos() {
    for (auto redo : this->redo_stack) {
        delete redo;
    }

    this->redo_stack.clear();
}

void Editor::doEntitySelection(Entity* entity) {
    // strand tool hacks
    if (this->selected_tool == EditorToolType::STRAND &&
        entity->getType() != EntityType::GOO_BALL)
        return;

    if (this->selected_tool == EditorToolType::STRAND &&
        this->strand_start_ball) {
        entity->setSelected(true);
        this->selected_entities.push_back(entity);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl)) {
        if (entity->getSelected()) {
            this->doAction(new DeselectEditorAction({entity}));
        } else {
            this->doAction(new SelectEditorAction({entity}));
        }
    } else {
        this->doAction(new SelectEditorAction(
            {entity}, {new DeselectEditorAction(this->selected_entities)}));
    }
}

void Editor::doOpenFile() {
    NFD_Init();
    nfdu8char_t* out_path;
    nfdu8filteritem_t filters[2] = {{"World Of Goo 2", "wog2"}};
    nfdopendialogu8args_t args = {0};
    args.filterList = filters;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&out_path, &args);
    if (result == NFD_OKAY) {
        LevelInfo level_info;
        std::string buffer;
        auto level_info_error =
            glz::read_file_json<glz::opts{.error_on_unknown_keys = false}>(
                level_info, out_path, buffer);

        if (level_info_error) {
            this->errors.push_back(JSONDeserializeError(
                out_path, glz::format_error(level_info_error, buffer)));
        } else {
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
    this->clearUndos();
    this->clearRedos();
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
                    auto ec = glz::write_file_json<glz::opts{.prettify = true}>(
                        this->level->getInfo(), this->level_file_path,
                        std::string{});
                }
            }

            ImGui::BeginDisabled(); // todo: make work
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
                auto ec = glz::write_file_json<glz::opts{.prettify = true}>(
                    this->level->info, "./obj.json", std::string{});
            }
            ImGui::EndDisabled();

            if (ImGui::MenuItem("Close")) {
                this->doCloseFile();
            }

            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                window.close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::BeginDisabled(this->selected_entities.empty());
            if (ImGui::MenuItem("Delete", "Del")) {
                this->doEntitiesDeletion(this->selected_entities);
            }
            ImGui::EndDisabled();

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

        if (ImGui::BeginMenu("Add")) {
            Vector2f center =
                Level::screenToWorld(this->window.getView().getCenter());

            if (ImGui::BeginMenu("GooBall")) {
                for (auto type : GooBall::ball_type_to_name) {
                    if (ImGui::MenuItem(type.second.c_str())) {
                        GooBall* goo_ball = new GooBall();
                        GooBallInfo info;
                        info.typeEnum = type.first;
                        goo_ball->setup(
                            this->level, info,
                            nullptr); // errors be damned, todo: handle
                        goo_ball->setPosition(center);

                        this->doAction(new CreateEditorAction(goo_ball));
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("ItemInstance")) {
                ItemInstance* item_instance = new ItemInstance();
                item_instance->setup(ItemInstanceInfo{});
                item_instance->setPosition(center);

                this->doAction(new CreateEditorAction(item_instance));
            }

            if (ImGui::MenuItem("TerrainGroup")) {
                TerrainGroup* terrain_group = new TerrainGroup();
                terrain_group->setup(TerrainGroupInfo{});
                terrain_group->setPosition(center);

                this->doAction(new CreateEditorAction(terrain_group));
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::registerErrorDialog() {
    if (ImGui::BeginPopupModal("Error", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string message = "";
        if (!this->errors.empty()) {
            Error error_value = this->errors.back();
            BaseError* base_error = std::visit(
                [](auto& derived_error) -> BaseError* {
                    return &derived_error;
                },
                error_value);
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

void Editor::showErrorDialog() { ImGui::OpenPopup("Error"); }

void Editor::registerSelectWOG2DirectoryDialog() {
    if (ImGui::BeginPopupModal("Select World of Goo 2 Install", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(
            "Please select the 'game' directory in your World of Goo 2 "
            "install:");

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
            if (ImGui::Selectable("", entity->getSelected(),
                                  ImGuiSelectableFlags_SpanAllColumns)) {
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

            ImGui::Image(entity->getThumbnail(),
                         ImVec2(textSize.y * 2.0f, textSize.y * 2.0f));
            ImGui::SameLine();
            ImGui::Text(text.c_str());

            if (entity->getType() == EntityType::GOO_BALL) {
                GooBall* goo_ball = static_cast<GooBall*>(entity);
                GooBallInfo& info = goo_ball->getInfo();
                GooBallInfo editor_info = info;
                bool modified = false;
                bool refresh = false;

                ImGui::SeparatorText("General");
                if (ImGui::BeginTable("General", 3,
                                      ImGuiTableFlags_SizingStretchProp)) {
                    this->registerPropertiesField<GooBallType>(
                        "Template",
                        [goo_ball] { return goo_ball->getBallType(); },
                        [goo_ball](GooBallType type) {
                            goo_ball->setBallType(type);
                        });

                    if (goo_ball->getBallType() == GooBallType::TERRAIN) {
                        this->registerPropertiesField<TerrainGroup*>(
                            "Terrain Group",
                            [goo_ball] { return goo_ball->getTerrainGroup(); },
                            [goo_ball](TerrainGroup* group) {
                                goo_ball->setTerrainGroup(group);
                            });
                    }

                    this->registerPropertiesField<Vector2f>(
                        "Position",
                        [goo_ball] { return goo_ball->getPosition(); },
                        [goo_ball](Vector2f position) {
                            goo_ball->setPosition(position);
                        });

                    this->registerPropertiesField<float>(
                        "Rotation",
                        [goo_ball] { return goo_ball->getRotation(); },
                        [goo_ball](float rotation) {
                            goo_ball->setRotation(rotation);
                        });

                    ImGui::EndTable();
                }
            } else if (entity->getType() == EntityType::ITEM_INSTANCE) {
                ItemInstance* item_instance =
                    static_cast<ItemInstance*>(entity);
                ItemInstanceInfo& info = item_instance->getInfo();

                ImGui::SeparatorText("General");
                if (ImGui::BeginTable("General", 3,
                                      ImGuiTableFlags_SizingStretchProp)) {
                    this->registerPropertiesField<std::string,
                                                  ItemTemplatePropertyTag>(
                        "Template",
                        [item_instance] {
                            return item_instance->getItemTemplateUUID();
                        },
                        [item_instance](std::string type) {
                            item_instance->setItemTemplateUUID(type);
                        },
                        {new ModifyPropertyEditorAction<int>(
                             [item_instance] {
                                 return item_instance
                                     ->getForcedRandomizationIndex();
                             },
                             [item_instance](int index) {
                                 item_instance->setForcedRandomizationIndex(
                                     index);
                             },
                             -1),
                         new ModifyPropertyEditorAction<
                             std::vector<ItemInstanceUserVariableInfo>>(
                             {[item_instance] {
                                  return item_instance->getUserVariableValues();
                              },
                              [item_instance](
                                  std::vector<ItemInstanceUserVariableInfo>
                                      values) {
                                  item_instance->setUserVariableValues(values);
                              },
                              {}})});

                    this->registerPropertiesField<Vector2f>(
                        "Position",
                        [item_instance] {
                            return item_instance->getPosition();
                        },
                        [item_instance](Vector2f position) {
                            item_instance->setPosition(position);
                        });

                    this->registerPropertiesField<Vector2f>(
                        "Scale",
                        [item_instance] { return item_instance->getScale(); },
                        [item_instance](Vector2f scale) {
                            item_instance->setScale(scale);
                        });

                    this->registerPropertiesField<float>(
                        "Rotation",
                        [item_instance] {
                            return item_instance->getRotation();
                        },
                        [item_instance](float rotation) {
                            item_instance->setRotation(rotation);
                        });

                    this->registerPropertiesField<float>(
                        "Depth",
                        [item_instance] { return item_instance->getDepth(); },
                        [item_instance](float depth) {
                            item_instance->setDepth(depth);
                        });

                    ImGui::EndTable();
                }

                ImGui::SeparatorText("User Variables");
                if (ImGui::BeginTable("User Variables", 3,
                                      ImGuiTableFlags_SizingStretchProp)) {
                    size_t var_i = 0;
                    for (auto var : item_instance->getUserVariableInfo()) {
                        if (var.enabled) {
                            if (var.type == ItemUserVariableType::FLOAT) {
                                this->registerPropertiesField<float>(
                                    var.name.c_str(),
                                    [item_instance, var_i] {
                                        return item_instance
                                            ->getUserVariableValue<float>(
                                                var_i);
                                    },
                                    [item_instance, var_i](float value) {
                                        item_instance
                                            ->setUserVariableValue<float>(
                                                var_i, value);
                                    });
                            } else if (var.type == ItemUserVariableType::INT) {
                                this->registerPropertiesField<int>(
                                    var.name.c_str(),
                                    [item_instance, var_i] {
                                        return item_instance
                                            ->getUserVariableValue<int>(var_i);
                                    },
                                    [item_instance, var_i](int value) {
                                        item_instance
                                            ->setUserVariableValue<int>(var_i,
                                                                        value);
                                    });
                            } else if (var.type == ItemUserVariableType::BOOL) {
                                this->registerPropertiesField<bool>(
                                    var.name.c_str(),
                                    [item_instance, var_i] {
                                        return item_instance
                                            ->getUserVariableValue<bool>(var_i);
                                    },
                                    [item_instance, var_i](bool value) {
                                        item_instance
                                            ->setUserVariableValue<bool>(var_i,
                                                                         value);
                                    });
                            } else if (var.type ==
                                       ItemUserVariableType::LIQUID_TYPE) {
                                this->registerPropertiesField<LiquidType>(
                                    var.name.c_str(),
                                    [item_instance, var_i] {
                                        return item_instance
                                            ->getUserVariableValue<LiquidType>(
                                                var_i);
                                    },
                                    [item_instance, var_i](LiquidType value) {
                                        item_instance
                                            ->setUserVariableValue<LiquidType>(
                                                var_i, value);
                                    });
                            } else if (var.type ==
                                       ItemUserVariableType::BALL_TYPE) {
                                this->registerPropertiesField<GooBallType>(
                                    var.name.c_str(),
                                    [item_instance, var_i] {
                                        return item_instance
                                            ->getUserVariableValue<GooBallType>(
                                                var_i);
                                    },
                                    [item_instance, var_i](GooBallType value) {
                                        item_instance
                                            ->setUserVariableValue<GooBallType>(
                                                var_i, value);
                                    });
                            }
                        }

                        ++var_i;
                    }

                    ImGui::EndTable();
                }
            } else if (entity->getType() == EntityType::TERRAIN_GROUP) {
                TerrainGroup* terrain_group =
                    static_cast<TerrainGroup*>(entity);

                ImGui::SeparatorText("General");
                if (ImGui::BeginTable("General", 3,
                                      ImGuiTableFlags_SizingStretchProp)) {
                    this->registerPropertiesField<std::string,
                                                  TerrainTemplatePropertyTag>(
                        "Template",
                        [terrain_group] {
                            return terrain_group->getTerrainTemplateUUID();
                        },
                        [terrain_group](std::string type) {
                            terrain_group->setTerrainTemplateUUID(type);
                        });

                    this->registerPropertiesField<float>(
                        "Depth",
                        [terrain_group] { return terrain_group->getDepth(); },
                        [terrain_group](float depth) {
                            terrain_group->setDepth(depth);
                        });

                    this->registerPropertiesField<int>(
                        "Sort Offset",
                        [terrain_group] {
                            return terrain_group->getSortOffset();
                        },
                        [terrain_group](int offset) {
                            terrain_group->setSortOffset(offset);
                        });

                    ImGui::EndTable();
                }
            }
        }
    }

    ImGui::End();
}

void Editor::registerResourcesWindow() {
    ImGui::Begin("Resources");

    ImGui::End();
}

void Editor::registerToolbarWindow() {
    ImGui::Begin("Toolbar");

    const size_t tool_count = Editor::tool_type_to_name.size();
    for (size_t i = 0; i < tool_count; ++i) {
        EditorToolType tool = static_cast<EditorToolType>(i);

        if (tool == this->selected_tool) {
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        }

        if (ImGui::Button(Editor::tool_type_to_name[tool])) {
            this->doAction(new SwitchToolEditorAction(
                tool, {new DeselectEditorAction(this->selected_entities)}));
        }

        ImGui::PopStyleColor();

        if (i < tool_count - 1) ImGui::SameLine();
    }

    ImGui::End();
}

// returns true if changed
bool Editor::registerGooBallTypeCombo(const char* label, GooBallType* type) {
    bool changed = false;
    if (ImGui::BeginCombo(label,
                          GooBall::ball_type_to_name.at(*type).c_str())) {
        for (auto& [name, id] : GooBall::ball_name_to_type) {
            if (name == "Invalid" || name == "LiquidLevelExit") {
                continue; // exclude these
            }

            bool selected = id == *type;
            if (ImGui::Selectable(name.c_str(), selected)) {
                *type = id;
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

void Editor::doEntitiesDeletion(std::vector<Entity*> entities) {
    std::set<Entity*> implicit_entities;

    for (auto entity : entities) {
        if (entity->getType() == EntityType::GOO_BALL) {
            auto ball = static_cast<GooBall*>(entity);

            for (auto strand : ball->getStrands()) {
                implicit_entities.insert(strand);
            }
        }
    }

    std::vector<Entity*> deleted;

    for (auto entity : implicit_entities) {
        deleted.push_back(entity);
    }

    for (auto entity : entities) {
        deleted.push_back(entity);
    }

    this->doAction(
        new DeleteEditorAction(deleted, {new DeselectEditorAction(entities)}));
}

std::unordered_map<EditorToolType, const char*> Editor::tool_type_to_name = {
    {EditorToolType::MOVE, "Move"}, {EditorToolType::STRAND, "Strand"}};

} // namespace gooforge