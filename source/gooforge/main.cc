// codeshaunted - gooforge
// source/gooforge/main.cc
// contains entry point
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

#include "boy_image.hh"

int main(int argc, char* argv[]) {
    gooforge::Editor editor;

    editor.initialize();

    return 0;

    /*
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gooforge");
    window.setFramerateLimit(GOOFORGE_FRAMERATE_LIMIT);
    ImGui::SFML::Init(window);

    // Create a view for the window
    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    window.setView(view);

    // Zoom and pan variables
    float zoomLevel = 1.0f;
    sf::Vector2f panOffset(0.0f, 0.0f);
    const float zoomFactor = 0.1f; // How much zoom changes with each step

    /*
    auto loadManifestResult = gooforge::ResourceManager::getInstance()->loadManifest("_atlas.image.atlas");

    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load("C:/Program Files/World of Goo 2/game/res/levels/C04_Gravitation_Navigation_Station.wog2");
    simdjson::ondemand::document level = parser.iterate(json);

    for (auto balldata : level.find_field("balls")) {
        auto ballstate = gooforge::GooBallState::deserialize(balldata.value());
        gooforge::GooBall* gooball = new gooforge::GooBall(ballstate);
        gooforge::EntityManager::getInstance()->add(gooball);
    }

    for (auto stranddata : level.find_field("strands")) {
        auto strandstate = gooforge::GooStrandState::deserialize(stranddata.value());
        gooforge::GooStrand* goostrand = new gooforge::GooStrand(strandstate);
        gooforge::EntityManager::getInstance()->add(goostrand);
    }
    

    sf::Clock deltaClock;
    while (window.isOpen()) {
        
        /*sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Handle panning and zooming
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    panOffset.y -= 30.0f;
                }
                if (event.key.code == sf::Keyboard::Down) {
                    panOffset.y += 30.0f;
                }
                if (event.key.code == sf::Keyboard::Left) {
                    panOffset.x -= 30.0f;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    panOffset.x += 30.0f;
                }
                if (event.key.code == sf::Keyboard::Z) {
                    zoomLevel *= (1.0f + zoomFactor);
                }
                if (event.key.code == sf::Keyboard::X) {
                    zoomLevel *= (1.0f - zoomFactor);
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        gooforge::EntityManager::getInstance()->update();

        // Update the view
        view.setCenter(view.getCenter() + panOffset);
        view.setSize(1920 / zoomLevel, 1080 / zoomLevel);
        window.setView(view);

        // Clear and draw
        window.clear(sf::Color::White);
        gooforge::EntityManager::getInstance()->draw(&window);
        ImGui::SFML::Render(window);
        window.display();

        // Reset panOffset to prevent continuous panning
        panOffset = sf::Vector2f(0.0f, 0.0f);
    }

    ImGui::SFML::Shutdown();

    return 0;
    */
}
