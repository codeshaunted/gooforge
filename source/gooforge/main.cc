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

#include "SFML/Graphics.hpp"

#include "entity_manager.hh"
#include "goo_ball.hh"
#include "resource_manager.hh"

#include <iostream>

#define GOOFORGE_PIXELS_PER_UNIT 113.385826772 // roughly 2160/19.05, calculated manually
#define GOOFORGE_FRAMERATE_LIMIT 60

int main(int argc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "gooforge");
    window.setFramerateLimit(GOOFORGE_FRAMERATE_LIMIT);

    gooforge::ResourceManager::getInstance()->loadManifest("_atlas.image.atlas");

    auto resource = gooforge::ResourceManager::getInstance()->getSpriteResource("IMAGE_BALL_COMMON_BODY");
    //auto resource2 = 

    // Create an SFML Sprite to display the texture
    //sf::Sprite sprite(*(*resource)->get());
    //sprite.scale(sf::Vector2f(0.5 * 0.300000011920929, 0.5 * 0.300000011920929));
    //sf::Sprite sprite2(*(*resource2)->get());
    //sprite2.scale(sf::Vector2f(0.5 * 0.300000011920929, 0.5 * 0.300000011920929));
    
    gooforge::EntityManager::getInstance()->add(new gooforge::GooBall());
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }


        window.clear(sf::Color::White);

        gooforge::EntityManager::getInstance()->draw(&window);

        window.display();
    }

    return 0;
}
