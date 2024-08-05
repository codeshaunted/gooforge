// codeshaunted - gooforge
// include/gooforge/goo_ball.hh
// contains GooBall declarations
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

#ifndef GOOFORGE_GOO_BALL_HH
#define GOOFORGE_GOO_BALL_HH

#include <unordered_map>

#include "SFML/Graphics.hpp"

#include "entity.hh"

namespace gooforge {

enum class GooBallType {
	INVALID = 0,
	COMMON,
	COMMON_ALBINO,
	IVY,
	BALLOON,
	GOOLF_SINGLE,
	ANCHOR,
	LAUNCHER_L2B,
	GOO_PRODUCT,
	THRUSTER,
	TERRAIN,
	BALLOON_EYE,
	CONDUIT,
	LAUNCHER_L2L,
	GOO_PRODUCT_WHITE,
	GROW,
	BOMB_STICKY,
	ROPE,
	BOUNCY,
	FISH,
	TIME_BUG,
	MATCH_STICK = 23,
	COMMON_WORLD,
	FIREWORKS,
	LIGHTBALL,
	TWU_BIT,
	TWU_BIT_BIT,
	ADAPTER,
	WINCH,
	SHRINK = 32,
	JELLY,
	GOOLF,
	THIS_WAY_UP,
	LIQUID_LEVEL_EXIT,
	EYE,
	UTIL_ATTACH_WALKABLE
};

class GooBall : public Entity {
	public:
		void update() override;
		void draw(sf::RenderWindow* window) override;
	private:
		static std::unordered_map<std::string, GooBallType> ball_name_to_type;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_BALL_HH