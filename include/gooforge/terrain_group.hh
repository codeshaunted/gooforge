// codeshaunted - gooforge
// include/gooforge/terrain_group.hh
// contains TerrainGroup declarations
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

#ifndef GOOFORGE_TERRAIN_GROUP_HH
#define GOOFORGE_TERRAIN_GROUP_HH

#include <expected>

#include "goo_ball.hh"

namespace gooforge {

struct TerrainGroupInfo {
	Vector2f textureOffset;
	std::string typeUuid;
	int typeIndex;
	int sortOffset;
	float depth;
	bool foreground;
	bool collision;
	bool destructable;
	bool buildable;
	bool occluder;
};

class TerrainGroup : public Entity {
	public:
		TerrainGroup() : Entity(EntityType::TERRAIN_GROUP) {}
		std::expected<void, Error> setup(TerrainGroupInfo* info);
		std::expected<void, Error> refresh();
		void update() override;
		void draw(sf::RenderWindow* window) override;
		void addTerrainBall(GooBall* goo_ball);
	private:
		TerrainGroupInfo* info;

		std::vector<GooBall*> terrain_balls;
};

} // namespace gooforge

#endif // GOOFORGE_TERRAIN_GROUP_HH