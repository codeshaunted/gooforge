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

    this->registerErrorDialog();
    this->registerSelectWOG2DirectoryDialog();
    this->registerMainMenuBar();

    /*
    ImGui::SetNextWindowPos(ImVec2(0, menu_bar_height)); // Position at the top-left corner
    ImGui::SetNextWindowSize(ImVec2(this->window.getSize().x / 5, this->window.getSize().y - menu_bar_height));
    ImGui::Begin("Level Tree", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    if (this->level) {
        for (auto entity : this->level->entities) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf;
            if (entity == this->selected_entity) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (ImGui::TreeNodeEx("ENTITY", flags)) {
                ImGui::TreePop();
            }
        }
    }

    ImGui::End();
    */

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

            /*if (event.mouseButton.button == sf::Mouse::Left) {
                if (this->level) {
                    sf::Vector2f screen_click_position = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), this->view);
                    Vector2f world_click_position = Level::screenToWorld(screen_click_position);

                    this->level->sortEntities();
                    for (auto entity : this->level->entities) {
                        if (entity->wasClicked(world_click_position)) {
                            this->selected_entity->setSelected(false);
                            this->selected_entity = entity;
                            entity->setSelected(true);
                            break;
                        }
                    }
                }
            }*/
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
                    // TODO: replace this with our own file load
                    auto level_info_result = LevelInfo::deserializeFile(out_path);

                    if (!level_info_result) {
                        this->error = level_info_result.error();
                    }
                    else {
                        this->level = new Level(level_info_result.value());
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

        ImGui::EndMainMenuBar();
    }
}

void Editor::registerErrorDialog() {
    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string message = "";
        if (this->error) {
            Error error_value = this->error.value();
            if (JSONDeserializeError* json_error = std::get_if<JSONDeserializeError>(&error_value)) {
                message = "Error deserializing JSON at field = '" + json_error->field + "', error = '" + simdjson::error_message(json_error->code) + "'";
            }
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
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Editor::showSelectWOG2DirectoryDialog() {
    ImGui::OpenPopup("Select World of Goo 2 Install");
}

} // namespace gooforge