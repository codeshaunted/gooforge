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

#include <format>

#include "imgui.h"
#include "imgui-SFML.h"
#include "nfd.h"
#include "simdjson.h"
#include "spdlog.h"

#include "constants.hh"

namespace gooforge {

void Editor::initialize() {
    this->window.create(sf::VideoMode(1920, 1080), "gooforge");
    this->window.setFramerateLimit(GOOFORGE_FRAMERATE_LIMIT);
    ImGui::SFML::Init(this->window);

    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    this->window.setView(view);

    sf::Clock delta_clock;
    while (this->window.isOpen()) {
        this->update(delta_clock);
        this->draw();
    }

    ImGui::SFML::Shutdown();
}

void Editor::update(sf::Clock delta_clock) {
    sf::Event event;
    while (this->window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(this->window, event);

        if (event.type == sf::Event::Closed) {
            this->window.close();
        }
    }

    ImGui::SFML::Update(this->window, delta_clock.restart());

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                NFD_Init();
                nfdu8char_t* out_path;
                nfdu8filteritem_t filters[2] = {{"World Of Goo 2", "wog2"}};
                nfdopendialogu8args_t args = {0};
                args.filterList = filters;
                args.filterCount = 1;
                nfdresult_t result = NFD_OpenDialogU8_With(&out_path, &args);
                if (result == NFD_OKAY) {
                    // TODO: replace this with our own file load
                    simdjson::ondemand::parser parser;
                    simdjson::padded_string json = simdjson::padded_string::load(out_path);
                    simdjson::ondemand::document document = parser.iterate(json);
                    auto level_info_result = LevelInfo::deserialize(document.get_value());

                    if (!level_info_result) {
                        this->error = level_info_result.error();
                    }
                    else {
                        for (auto ball : level_info_result.value().balls) {
                            spdlog::info("Ball UID: {}", ball.uid);
                        }
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

        if (error) {
            ImGui::OpenPopup("Error");
        }

        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            std::string message = "";
            std::shared_ptr<Error> error_value = error.value();
            switch (error_value->type) {
                case ErrorType::JSON_DESERIALIZE: {
                    std::shared_ptr<JSONDeserializeError> json_error = std::static_pointer_cast<JSONDeserializeError>(error_value);
                    message = "Error deserializing JSON at field = '" + json_error->field + "', error = '" + simdjson::error_message(json_error->code) + "'";
                    break;
                }
                default:
                    break;
            }

            ImGui::Text(message.c_str());
            ImGui::Separator();

            if (!this->error_logged) {
                spdlog::error(message);
                this->error_logged = true;
            }

            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                this->error = std::nullopt;
                this->error_logged = false;
            }

            ImGui::EndPopup();
        }

        ImGui::EndMainMenuBar();
    }

    if (!std::filesystem::exists(this->wog2_path)) {
        this->showSelectWOG2DirectoryDialog();
    }

    //EntityManager::getInstance()->update();
}

void Editor::draw() {
    this->window.clear(sf::Color::White);
    //gooforge::EntityManager::getInstance()->draw(&this->window);
    ImGui::SFML::Render(this->window);
    this->window.display();
}

void Editor::showSelectWOG2DirectoryDialog() {
    ImGui::OpenPopup("Select World of Goo 2 Install");

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
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Editor::showErrorDialog(LegacyError error) {
    ImGui::OpenPopup("Error");

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string message = "An error occurred! Code: " + std::to_string(static_cast<uint32_t>(error));
        ImGui::Text(message.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

} // namespace gooforge