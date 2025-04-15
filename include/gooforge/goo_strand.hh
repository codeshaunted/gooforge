// codeshaunted - gooforge
// include/gooforge/goo_strand.hh
// contains GooStrand declarations
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

#ifndef GOOFORGE_GOO_STRAND_HH
#define GOOFORGE_GOO_STRAND_HH

#include "SFML/Graphics.hpp"

#include "entity.hh"
#include "goo_ball.hh"

namespace gooforge {

struct GooStrandInfo {
	unsigned int ball1UID;
	unsigned int ball2UID;
	GooBallType type;
	bool filled;
};

class GooStrand : public Entity {
	public:
		GooStrand() : Entity(EntityType::GOO_STRAND) {}
		~GooStrand() override;
		std::expected<void, Error> setup(GooStrandInfo info, GooBall* ball1, GooBall* ball2);
		std::expected<void, Error> refresh() override;
		sf::Sprite getThumbnail() override;
		std::string getDisplayName() override;
		Vector2f getPosition() override;
		void update() override;
		void draw(sf::RenderWindow* window) override;
	private:
		GooBall* ball1;
		GooBall* ball2;
		GooStrandInfo info;
		BallTemplateInfo* ball_template = nullptr;
		sf::Sprite display_sprite;

	friend class TerrainGroup;
	friend class Level;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_STRAND_HH