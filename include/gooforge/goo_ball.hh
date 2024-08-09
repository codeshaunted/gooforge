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
#include "goo_ball_template.hh"

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

struct GooBallInfo {
	GooBallType typeEnum;
	int uid;
	Vector2f pos;
	float angle;
	bool discovered;
	bool floatingWhileAsleep;
	bool interactive;
	bool wakeWithLiquid;
	bool exitPipeAlert;
	bool affectsAutoBounds;
	float launcherLifespanMin;
	float launcherLifespanMax;
	float launcherForceFactor;
	bool launcherCanUseBalls;
	float launcherKnockbackFactor;
	int launcherMaxActive;
	int launcherBallTypeToGenerate;
	float thrustForce;
	float maxVelocity;
	float stiffness;
	bool filled;
	float detonationRadius;
	float detonationForce;
};


class GooBall : public Entity {
	public:
		GooBall(GooBallInfo* info);
		~GooBall();
		void update() override;
		void draw(sf::RenderWindow* window) override;
		static void loadGooBallTemplates(std::string_view path);
		GooBallInfo* getInfo();
		GooBallTemplate* getTemplate();
	private:
		static std::unordered_map<std::string, GooBallType> ball_name_to_type;
		static std::unordered_map<GooBallType, GooBallTemplate> ball_templates;
		static std::unordered_map<unsigned int, GooBall*> balls;
		GooBallInfo* info;
		GooBallTemplate* ball_template = nullptr;

	friend class GooStrand;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_BALL_HH