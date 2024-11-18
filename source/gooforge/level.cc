// codeshaunted - gooforge
// source/gooforge/level.cc
// contains Level definitions
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

#include "level.hh"

#include <fstream>
#include <numbers>
#include <sstream>

#include "spdlog.h"

#include "constants.hh"

namespace gooforge {

std::expected<void, Error> Level::setup(LevelInfo info) {
	this->info = info;

	for (ItemInstanceInfo& item_instance_info : this->info.items) {
		ItemInstance* item_instance = new ItemInstance();
		auto result = item_instance->setup(&item_instance_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->addEntity(item_instance);
	}

	size_t goo_ball_i = 0;
	for (GooBallInfo& ball_info : this->info.balls) {
		GooBall* goo_ball = new GooBall();
		auto result = goo_ball->setup(&ball_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->addEntity(goo_ball);
	}

	for (GooStrandInfo& strand_info : this->info.strands) {
		//this->addEntity(new GooStrand(&strand_info));
	}
	
	// TODO: MAKE THIS COLLECT ERRORS INSTEAD OF JUST RETURNING A SINGLE ONE
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

	for (auto it = this->entities.rbegin(); it != this->entities.rend(); ++it) {
    	Entity* entity = *it;
    	entity->draw(window);
	}
}

sf::Vector2f Level::worldToScreen(Vector2f world) {
	return sf::Vector2f(world.x * GOOFORGE_PIXELS_PER_UNIT, -1.0f * world.y * GOOFORGE_PIXELS_PER_UNIT);
}

Vector2f Level::screenToWorld(sf::Vector2f screen) {
	return Vector2f(screen.x / GOOFORGE_PIXELS_PER_UNIT, -1.0f * (screen.y / GOOFORGE_PIXELS_PER_UNIT));
}

float Level::radiansToDegrees(float radians) {
	return radians * (180.0f / std::numbers::pi);
}

float Level::degreesToRadians(float degrees) {
	return degrees * (std::numbers::pi / 180.0f);
}

void Level::sortEntities() {
	if (!this->entities_dirty) {
		return;
	}

	std::sort(this->entities.begin(), this->entities.end(), [](const Entity* a, const Entity* b) {
		return a->depth < b->depth;
	});

	this->entities_dirty = false;
}

} // namespace gooforge