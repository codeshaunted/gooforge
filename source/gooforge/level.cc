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

	std::vector<GooBall*> goo_balls;
	std::unordered_map<int, GooBall*> goo_balls_uid;
	for (GooBallInfo& ball_info : this->info.balls) {
		GooBall* goo_ball = new GooBall();
		goo_balls.push_back(goo_ball);
		goo_balls_uid.insert({ball_info.uid, goo_ball});
		auto result = goo_ball->setup(&ball_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->addEntity(goo_ball);
	}

	std::vector<TerrainGroup*> terrain_groups;
	for (TerrainGroupInfo& terrain_group_info : this->info.terrainGroups) {
		TerrainGroup* terrain_group = new TerrainGroup();
		terrain_groups.push_back(terrain_group);
		auto result = terrain_group->setup(&terrain_group_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->addEntity(terrain_group);
	}

	for (size_t i = 0; i < this->info.terrainBalls.size(); ++i) {
		const TerrainBallInfo& terrain_ball_info = this->info.terrainBalls[i];

		if (terrain_ball_info.group != -1) {
			terrain_groups[terrain_ball_info.group]->addTerrainBall(goo_balls[i]); // TODO: FIX THIS, THIS IS UNSAFE WE MUST VALIDATE THIS FIRST
		}
	}

	for (GooStrandInfo& strand_info : this->info.strands) {
		GooStrand* goo_strand = new GooStrand();
		// ONCE AGAIN NOT SAFE, TODO: FIX
		auto result = goo_strand->setup(&strand_info, goo_balls_uid[strand_info.ball1UID], goo_balls_uid[strand_info.ball2UID]);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->addEntity(goo_strand);
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