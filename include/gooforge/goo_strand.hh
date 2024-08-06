// codeshaunted - gooforge
// include/gooforge/goo_strand.hh
// contains GooStrand declarations
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

	static GooStrandInfo deserialize(simdjson::ondemand::value json);
};


class GooStrand : public Entity {
	public:
		GooStrand(GooStrandInfo state);
		void update() override;
		void draw(sf::RenderWindow* window) override;
	private:
		GooBall* ball1;
		GooBall* ball2;
		GooStrandInfo info;
		GooBallTemplate* ball_template = nullptr;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_STRAND_HH