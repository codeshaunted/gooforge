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
		auto item_instance = std::make_shared<ItemInstance>();
		auto result = item_instance->setup(item_instance_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->entities.insert(item_instance);
	}

	std::vector<std::shared_ptr<TerrainGroup>> terrain_groups_indexed;
	for (TerrainGroupInfo& terrain_group_info : this->info.terrainGroups) {
		auto terrain_group = std::make_shared<TerrainGroup>();

		auto result = terrain_group->setup(terrain_group_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->entities.insert(terrain_group);
		terrain_groups_indexed.push_back(terrain_group);
	}

	std::vector<std::shared_ptr<GooBall>> goo_balls;
	std::unordered_map<int, std::shared_ptr<GooBall>> goo_balls_uid;
	size_t i = 0;
	for (GooBallInfo& ball_info : this->info.balls) {
		auto goo_ball = std::make_shared<GooBall>();
		goo_balls.push_back(goo_ball);
		goo_balls_uid.insert({ball_info.uid, goo_ball});

		size_t terrain_group_index = this->info.terrainBalls[i].group;
		auto terrain_group = terrain_group_index == -1 ? nullptr : terrain_groups_indexed[terrain_group_index];
		auto result = goo_ball->setup(ball_info, terrain_group);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->entities.insert(goo_ball);
		++i;
	}

	for (GooStrandInfo& strand_info : this->info.strands) {
		auto goo_strand = std::make_shared<GooStrand>();
		// ONCE AGAIN NOT SAFE, TODO: FIX
		auto result = goo_strand->setup(strand_info, goo_balls_uid[strand_info.ball1UID], goo_balls_uid[strand_info.ball2UID]);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->entities.insert(goo_strand);

		std::shared_ptr<GooBall> ball1 = goo_balls_uid[strand_info.ball1UID];
		std::shared_ptr<GooBall> ball2 = goo_balls_uid[strand_info.ball2UID];
		ball1->addStrand(goo_strand);
		ball2->addStrand(goo_strand);

		this->addStrand(goo_strand);
	}

	return std::expected<void, Error>{};
}

LevelInfo& Level::getInfo() {
	// rebuilds info before returning
	this->info.items.clear();
	this->info.balls.clear();
	this->info.strands.clear();
	this->info.terrainGroups.clear();
	this->info.terrainBalls.clear();

	int terrain_group_i = 0;
	std::unordered_map<std::shared_ptr<TerrainGroup>, int> terrain_groups;
	terrain_groups.insert({nullptr, -1});
	int next_uid = 0;
	for (std::shared_ptr<Entity> entity : this->entities) {
		switch (entity->getType()) {
			case EntityType::GOO_BALL: {
				std::shared_ptr<GooBall> ball = static_pointer_cast<GooBall>(entity);
				GooBallInfo& ball_info = ball->getInfo();
				ball_info.uid = next_uid;
				++next_uid;

				this->info.balls.push_back(ball_info);
				break;
			}
			case EntityType::ITEM_INSTANCE: {
				std::shared_ptr<ItemInstance> item = static_pointer_cast<ItemInstance>(entity);
				ItemInstanceInfo& item_info = item->getInfo();
				item_info.uid = next_uid;
				++next_uid;

				this->info.items.push_back(item_info);
				break;
			}
			case EntityType::TERRAIN_GROUP: {
				std::shared_ptr<TerrainGroup> terrain_group = static_pointer_cast<TerrainGroup>(entity);
				terrain_groups.insert({terrain_group, terrain_group_i});
				++terrain_group_i;

				this->info.terrainGroups.push_back(terrain_group->getInfo());
			}
		}
	}

	// we need to build strands after the balls have been assigned their uids
	for (std::shared_ptr<Entity> entity : this->entities) {
		if (entity->getType() != EntityType::GOO_STRAND) continue;

		auto strand = static_pointer_cast<GooStrand>(entity);
		
		strand->info.ball1UID = strand->getBall1().lock()->getInfo().uid;
		strand->info.ball2UID = strand->getBall2().lock()->getInfo().uid;

		this->info.strands.push_back(strand->info);
	}

	// build terrainBalls
	for (std::shared_ptr<Entity> entity : this->entities) {
		if (entity->getType() != EntityType::GOO_BALL) continue;

		std::shared_ptr<GooBall> ball = static_pointer_cast<GooBall>(entity);

		int terrain_group_index = terrain_groups[ball->terrain_group.lock()]; // not necessarily safe?

		this->info.terrainBalls.push_back(TerrainBallInfo(terrain_group_index));
	}

	return this->info;
}

Level::~Level() {

}

void Level::update() {
}

void Level::draw(sf::RenderWindow* window) {
	for (std::shared_ptr<Entity> entity : this->entities) {
		entity->draw(window);
	}

	// draw select boxes over everything else
	// maybe this shouldn't be the case?
	for (std::shared_ptr<Entity> entity : this->entities) {
		if (!entity->getSelected()) continue;

		entity->drawSelection(window);
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

void Level::removeEntity(std::shared_ptr<Entity> entity) {
	switch (entity->getType()) {
		case EntityType::GOO_BALL:
			this->removeBall(std::static_pointer_cast<GooBall>(entity));
			break;
		case EntityType::GOO_STRAND:
			this->removeStrand(std::static_pointer_cast<GooStrand>(entity));
			break;
		case EntityType::ITEM_INSTANCE:
			this->entities.erase(entity);
			break;
		default:
			break;
	}
}

void Level::addBall(std::shared_ptr<GooBall> ball) {
	for (std::shared_ptr<Entity> entity : this->entities) {
		entity->notifyAddBall(ball);
	}

	this->entities.insert(ball);
}

void Level::removeBall(std::shared_ptr<GooBall> ball) {
	for (std::shared_ptr<Entity> entity : this->entities) {
		entity->notifyRemoveBall(ball);
	}

	this->entities.erase(ball);
}

void Level::addStrand(std::shared_ptr<GooStrand> strand) {
	for (std::shared_ptr<Entity> entity : this->entities) {
		entity->notifyAddStrand(strand);
	}

	this->entities.insert(strand);
}

void Level::removeStrand(std::shared_ptr<GooStrand> strand) {
	for (std::shared_ptr<Entity> entity : this->entities) {
		entity->notifyRemoveStrand(strand);
	}

	this->entities.erase(strand);
}

} // namespace gooforge