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

		this->item_instances.insert(item_instance);
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

		this->goo_balls.insert(goo_ball);
	}

	for (GooStrandInfo& strand_info : this->info.strands) {
		GooStrand* goo_strand = new GooStrand();
		// ONCE AGAIN NOT SAFE, TODO: FIX
		auto result = goo_strand->setup(&strand_info, goo_balls_uid[strand_info.ball1UID], goo_balls_uid[strand_info.ball2UID]);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->goo_strands.insert(goo_strand);

		this->goo_matrix[goo_balls_uid[strand_info.ball1UID]][goo_balls_uid[strand_info.ball2UID]] = goo_strand;
		this->goo_matrix[goo_balls_uid[strand_info.ball2UID]][goo_balls_uid[strand_info.ball1UID]] = goo_strand;

		goo_balls_uid[strand_info.ball1UID]->addStrand(goo_strand);
		goo_balls_uid[strand_info.ball2UID]->addStrand(goo_strand);
	}

	std::vector<TerrainGroup*> terrain_groups_indexed;
	for (TerrainGroupInfo& terrain_group_info : this->info.terrainGroups) {
		TerrainGroup* terrain_group = new TerrainGroup();

		auto result = terrain_group->setup(&terrain_group_info);
		if (!result) {
			return std::unexpected(result.error());
		}

		this->terrain_groups.insert(terrain_group);
		terrain_groups_indexed.push_back(terrain_group);
	}


	for (size_t i = 0; i < this->info.terrainBalls.size(); ++i) {
		const TerrainBallInfo& terrain_ball_info = this->info.terrainBalls[i];

		if (terrain_ball_info.group != -1) {
			terrain_groups_indexed[terrain_ball_info.group]->addTerrainBall(goo_balls[i]); // TODO: FIX THIS, THIS IS UNSAFE WE MUST VALIDATE THIS FIRST

			for (auto strand : goo_balls[i]->strands) {
				if (strand->info->type == GooBallType::TERRAIN) {
					terrain_groups_indexed[terrain_ball_info.group]->addTerrainStrand(strand);
				}
			}
		}
	}
}

Level::~Level() {

}

void Level::update() {
}

void Level::draw(sf::RenderWindow* window) {
	for (ItemInstance* item_instance : this->item_instances) {
		item_instance->draw(window);
	}

	for (TerrainGroup* terrain_group : this->terrain_groups) {
		terrain_group->draw(window);
	}

	for (GooStrand* goo_strand : this->goo_strands) {
		goo_strand->draw(window);
	}

	for (GooBall* goo_ball : this->goo_balls) {
		goo_ball->draw(window);
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

} // namespace gooforge