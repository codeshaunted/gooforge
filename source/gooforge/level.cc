// codeshaunted - gooforge
// source/gooforge/level.cc
// contains Level definitions
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

#include "level.hh"

#include <fstream>
#include <sstream>

#include "spdlog.h"

#include "constants.hh"

namespace gooforge {

Level::Level(LevelInfo info) {
	this->info = info;

	for (ItemInstanceInfo& item_instance_info : this->info.items) {
		this->addEntity(new ItemInstance(&item_instance_info));
	}

	for (GooBallInfo& ball_info : this->info.balls) {
		this->addEntity(new GooBall(&ball_info));
	}

	for (GooStrandInfo& strand_info : this->info.strands) {
		this->addEntity(new GooStrand(&strand_info));
	}
}

Level::~Level() {
	for (auto& entity : this->entities) {
		delete entity;
	}
}

// takes ownership of the entity
void Level::addEntity(Entity* entity) {
	this->entities.push_back(std::move(entity));
	this->entities_dirty = true;
}

void Level::update() {
	for (auto& entity : this->entities) {
		entity->update();
	}
}

void Level::draw(sf::RenderWindow* window) {
	this->sortEntities();

	for (auto& entity : this->entities) {
		entity->draw(window);
	}
}

sf::Vector2f Level::worldToScreen(Vector2f world) {
	return sf::Vector2f(world.x * GOOFORGE_PIXELS_PER_UNIT, -1.0f * world.y * GOOFORGE_PIXELS_PER_UNIT);
}

Vector2f Level::screenToWorld(sf::Vector2f screen) {
	return Vector2f(screen.x / GOOFORGE_PIXELS_PER_UNIT, -1.0f * (screen.y / GOOFORGE_PIXELS_PER_UNIT));
}

void Level::sortEntities() {
	if (!this->entities_dirty) {
		return;
	}

	std::sort(this->entities.begin(), this->entities.end(), [](const Entity* a, const Entity* b) {
		return a->layer < b->layer;
		});

	this->entities_dirty = false;
}

} // namespace gooforge