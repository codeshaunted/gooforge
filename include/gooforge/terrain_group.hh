// codeshaunted - gooforge
// include/gooforge/terrain_group.hh
// contains TerrainGroup declarations
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