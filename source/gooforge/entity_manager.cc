// codeshaunted - gooforge
// source/gooforge/entity_manager.cc
// contains EntityManager definitions
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

#include "entity_manager.hh"

namespace gooforge {

EntityManager* EntityManager::getInstance() {
    if (!EntityManager::instance) {
        EntityManager::instance = new EntityManager();
    }

    return EntityManager::instance;
}

void EntityManager::add(Entity* entity) {
    this->entities.push_back(entity);
    this->entities_dirty = true;
}

void EntityManager::update() {
    for (auto entity : this->entities) {
        entity->update();
    }
}

void EntityManager::draw(sf::RenderWindow* window) {
    if (this->entities_dirty) {
        std::sort(this->entities.begin(), this->entities.end(), [](const Entity* a, const Entity* b) {
            return a->z_index < b->z_index;
        });

        this->entities_dirty = false;
    }

    for (auto entity : this->entities) {
        entity->draw(window);
    }
}

EntityManager* EntityManager::instance = nullptr;

} // namespace gooforge