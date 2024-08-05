// codeshaunted - gooforge
// source/gooforge/goo_ball.cc
// contains GooBall definitions
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

#include "goo_ball.hh"

#include <filesystem>

#include "constants.hh"
#include "resource_manager.hh"

namespace gooforge {

GooBallState GooBallState::deserialize(simdjson::ondemand::value json) {
	GooBallState state;
	state.typeEnum = GooBallType(json.find_field("typeEnum").get_int64().take_value());
	state.uid = json.find_field("uid").get_int64().take_value();
	auto pos = json.find_field("pos");
	state.pos.x = pos.find_field("x").get_double().take_value();
	state.pos.y = pos.find_field("y").get_double().take_value();
	state.angle = json.find_field("angle").get_double().take_value();
	state.discovered = json.find_field("discovered").get_bool().take_value();
	state.floatingWhileAsleep = json.find_field("floatingWhileAsleep").get_bool().take_value();
	state.interactive = json.find_field("interactive").get_bool().take_value();
	state.wakeWithLiquid = json.find_field("wakeWithLiquid").get_bool().take_value();
	state.exitPipeAlert = json.find_field("exitPipeAlert").get_bool().take_value();
	state.affectsAutoBounds = json.find_field("affectsAutoBounds").get_bool().take_value();
	state.launcherLifespanMin = json.find_field("launcherLifespanMin").get_double().take_value();
	state.launcherLifespanMax = json.find_field("launcherLifespanMax").get_double().take_value();
	state.launcherForceFactor = json.find_field("launcherForceFactor").get_double().take_value();
	state.launcherCanUseBalls = json.find_field("launcherCanUseBalls").get_bool().take_value();
	state.launcherKnockbackFactor = json.find_field("launcherKnockbackFactor").get_double().take_value();
	state.launcherMaxActive = json.find_field("launcherMaxActive").get_int64().take_value();
	state.launcherBallTypeToGenerate = json.find_field("launcherBallTypeToGenerate").get_int64().take_value();
	state.thrustForce = json.find_field("thrustForce").get_double().take_value();
	state.maxVelocity = json.find_field("maxVelocity").get_double().take_value();
	state.stiffness = json.find_field("stiffness").get_double().take_value();
	state.filled = json.find_field("filled").get_bool().take_value();
	state.detonationRadius = json.find_field("detonationRadius").get_double().take_value();
	state.detonationForce = json.find_field("detonationForce").get_double().take_value();

	return state;
}

GooBall::GooBall(GooBallState state) {
	this->state = state;
	this->z_index = 1;

	if (!this->ball_template) {
		if (GooBall::ball_templates.empty()) { // empty could signify a failed load, TODO: fix
			GooBall::loadGooBallTemplates("C:/Program Files/World of Goo 2/game/res/balls");
		}

		if (!GooBall::ball_templates.contains(this->state.typeEnum)) {
			throw std::runtime_error("GooBall::GooBall GooBallTemplate for GooBallType = " + std::to_string(static_cast<int>(this->state.typeEnum)) + " has not been loaded");
		}

		this->ball_template = &GooBall::ball_templates.at(this->state.typeEnum);
	}

	GooBall::balls.insert({ this->state.uid, this });
}

void GooBall::update() {
	this->position = this->state.pos;
}

void GooBall::draw(sf::RenderWindow* window) {
	sf::Sprite sprite = *(*ResourceManager::getInstance()->getSpriteResource(this->ball_template->body_image_id))->get();

	// Set the origin to the center of the sprite
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	float scale = 0.5 * this->ball_template->width;
	sprite.setScale(sf::Vector2f(scale, scale));
	Vector2f screen_position = this->position.scale(GOOFORGE_PIXELS_PER_UNIT);
	sprite.setPosition(sf::Vector2f(screen_position.x, -1.0 * screen_position.y));
	window->draw(sprite);
}

void GooBall::loadGooBallTemplates(std::string_view path) {
	for (auto& ball : GooBall::ball_name_to_type) {
		std::string template_path = std::string(path) + "/" + ball.first + "/ball.wog2";
		if (std::filesystem::exists(template_path)) {
			GooBall::ball_templates.insert({ball.second, GooBallTemplate::deserializeFromFile(template_path)});
		}
	}
}

std::unordered_map<std::string, GooBallType> GooBall::ball_name_to_type = {
	{"Invalid", GooBallType::INVALID},
	{"Common", GooBallType::COMMON},
	{"CommonAlbino", GooBallType::COMMON_ALBINO},
	{"Ivy", GooBallType::IVY},
	{"Balloon", GooBallType::BALLOON},
	{"GoolfSingle", GooBallType::GOOLF_SINGLE},
	{"Anchor", GooBallType::ANCHOR},
	{"LauncherL2B", GooBallType::LAUNCHER_L2B},
	{"GooProduct", GooBallType::GOO_PRODUCT},
	{"Thruster", GooBallType::THRUSTER},
	{"Terrain", GooBallType::TERRAIN},
	{"BalloonEye", GooBallType::BALLOON_EYE},
	{"Conduit", GooBallType::CONDUIT},
	{"LauncherL2L", GooBallType::LAUNCHER_L2L},
	{"GooProductWhite", GooBallType::GOO_PRODUCT_WHITE},
	{"Grow", GooBallType::GROW},
	{"BombSticky", GooBallType::BOMB_STICKY},
	{"Rope", GooBallType::ROPE},
	{"Bouncy", GooBallType::BOUNCY},
	{"Fish", GooBallType::FISH},
	{"TimeBug", GooBallType::TIME_BUG},
	{"MatchStick", GooBallType::MATCH_STICK},
	{"CommonWorld", GooBallType::COMMON_WORLD},
	{"Fireworks", GooBallType::COMMON_WORLD},
	{"Lightball", GooBallType::LIGHTBALL},
	{"TwuBit", GooBallType::TWU_BIT},
	{"TwuBitBit", GooBallType::TWU_BIT_BIT},
	{"Adapter", GooBallType::ADAPTER},
	{"Winch", GooBallType::WINCH},
	{"Shrink", GooBallType::SHRINK},
	{"Jelly", GooBallType::JELLY},
	{"Goolf", GooBallType::GOOLF},
	{"ThisWayUp", GooBallType::THIS_WAY_UP},
	{"LiquidLevelExit", GooBallType::LIQUID_LEVEL_EXIT},
	{"Eye", GooBallType::EYE},
	{"UtilAttachWalkable", GooBallType::UTIL_ATTACH_WALKABLE}
};

std::unordered_map<GooBallType, GooBallTemplate> GooBall::ball_templates;

std::unordered_map<unsigned int, GooBall*> GooBall::balls;

} // namespace gooforge