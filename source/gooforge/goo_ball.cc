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

#include "spdlog.h"

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

std::expected<GooBallInfo, Error> GooBallInfo::deserialize(simdjson::ondemand::value json) {
	GooBallInfo info;

	auto typeEnum = json.find_field("typeEnum").get_int64();
	if (simdjson::error_code error = typeEnum.error()) {
		return std::unexpected(JSONDeserializeError("typeEnum", error));
	}
	info.typeEnum = GooBallType(typeEnum.value());

	auto uid = json.find_field("uid").get_int64();
	if (simdjson::error_code error = uid.error()) {
		return std::unexpected(JSONDeserializeError("uid", error));
	}
	info.uid = uid.value();

	auto pos = Vector2f::deserialize(json.find_field("pos"));
	if (!pos) {
		JSONDeserializeError error = std::get<JSONDeserializeError>(pos.error());
		error.prependField("pos");
		return std::unexpected(error);
	}
	info.pos = pos.value();

	auto angle = json.find_field("angle").get_double();
	if (simdjson::error_code error = angle.error()) {
		return std::unexpected(JSONDeserializeError("angle", error));
	}
	info.angle = angle.value();

	auto discovered = json.find_field("discovered").get_bool();
	if (simdjson::error_code error = discovered.error()) {
		return std::unexpected(JSONDeserializeError("discovered", error));
	}
	info.discovered = discovered.value();

	auto floatingWhileAsleep = json.find_field("floatingWhileAsleep").get_bool();
	if (simdjson::error_code error = floatingWhileAsleep.error()) {
		return std::unexpected(JSONDeserializeError("floatingWhileAsleep", error));
	}
	info.floatingWhileAsleep = floatingWhileAsleep.value();

	auto interactive = json.find_field("interactive").get_bool();
	if (simdjson::error_code error = interactive.error()) {
		return std::unexpected(JSONDeserializeError("interactive", error));
	}
	info.interactive = interactive.value();

	auto wakeWithLiquid = json.find_field("wakeWithLiquid").get_bool();
	if (simdjson::error_code error = wakeWithLiquid.error()) {
		return std::unexpected(JSONDeserializeError("wakeWithLiquid", error));
	}
	info.wakeWithLiquid = wakeWithLiquid.value();

	auto exitPipeAlert = json.find_field("exitPipeAlert").get_bool();
	if (simdjson::error_code error = exitPipeAlert.error()) {
		return std::unexpected(JSONDeserializeError("exitPipeAlert", error));
	}
	info.exitPipeAlert = exitPipeAlert.value();

	auto affectsAutoBounds = json.find_field("affectsAutoBounds").get_bool();
	if (simdjson::error_code error = affectsAutoBounds.error()) {
		return std::unexpected(JSONDeserializeError("affectsAutoBounds", error));
	}
	info.affectsAutoBounds = affectsAutoBounds.value();

	auto launcherLifespanMin = json.find_field("launcherLifespanMin").get_double();
	if (simdjson::error_code error = launcherLifespanMin.error()) {
		return std::unexpected(JSONDeserializeError("launcherLifespanMin", error));
	}
	info.launcherLifespanMin = launcherLifespanMin.value();

	auto launcherLifespanMax = json.find_field("launcherLifespanMax").get_double();
	if (simdjson::error_code error = launcherLifespanMax.error()) {
		return std::unexpected(JSONDeserializeError("launcherLifespanMax", error));
	}
	info.launcherLifespanMax = launcherLifespanMax.value();

	auto launcherForceFactor = json.find_field("launcherForceFactor").get_double();
	if (simdjson::error_code error = launcherForceFactor.error()) {
		return std::unexpected(JSONDeserializeError("launcherForceFactor", error));
	}
	info.launcherForceFactor = launcherForceFactor.value();

	auto launcherCanUseBalls = json.find_field("launcherCanUseBalls").get_bool();
	if (simdjson::error_code error = launcherCanUseBalls.error()) {
		return std::unexpected(JSONDeserializeError("launcherCanUseBalls", error));
	}
	info.launcherCanUseBalls = launcherCanUseBalls.value();

	auto launcherKnockbackFactor = json.find_field("launcherKnockbackFactor").get_double();
	if (simdjson::error_code error = launcherKnockbackFactor.error()) {
		return std::unexpected(JSONDeserializeError("launcherKnockbackFactor", error));
	}
	info.launcherKnockbackFactor = launcherKnockbackFactor.value();

	auto launcherMaxActive = json.find_field("launcherMaxActive").get_int64();
	if (simdjson::error_code error = launcherMaxActive.error()) {
		return std::unexpected(JSONDeserializeError("launcherMaxActive", error));
	}
	info.launcherMaxActive = launcherMaxActive.value();

	auto launcherBallTypeToGenerate = json.find_field("launcherBallTypeToGenerate").get_int64();
	if (simdjson::error_code error = launcherBallTypeToGenerate.error()) {
		return std::unexpected(JSONDeserializeError("launcherBallTypeToGenerate", error));
	}
	info.launcherBallTypeToGenerate = launcherBallTypeToGenerate.value();

	auto thrustForce = json.find_field("thrustForce").get_double();
	if (simdjson::error_code error = thrustForce.error()) {
		return std::unexpected(JSONDeserializeError("thrustForce", error));
	}
	info.thrustForce = thrustForce.value();

	auto maxVelocity = json.find_field("maxVelocity").get_double();
	if (simdjson::error_code error = maxVelocity.error()) {
		return std::unexpected(JSONDeserializeError("maxVelocity", error));
	}
	info.maxVelocity = maxVelocity.value();

	auto stiffness = json.find_field("stiffness").get_double();
	if (simdjson::error_code error = stiffness.error()) {
		return std::unexpected(JSONDeserializeError("stiffness", error));
	}
	info.stiffness = stiffness.value();

	auto filled = json.find_field("filled").get_bool();
	if (simdjson::error_code error = filled.error()) {
		return std::unexpected(JSONDeserializeError("filled", error));
	}
	info.filled = filled.value();

	auto detonationRadius = json.find_field("detonationRadius").get_double();
	if (simdjson::error_code error = detonationRadius.error()) {
		return std::unexpected(JSONDeserializeError("detonationRadius", error));
	}
	info.detonationRadius = detonationRadius.value();

	auto detonationForce = json.find_field("detonationForce").get_double();
	if (simdjson::error_code error = detonationForce.error()) {
		return std::unexpected(JSONDeserializeError("detonationForce", error));
	}
	info.detonationForce = detonationForce.value();

	return info;
}

GooBall::GooBall(GooBallInfo* info) {
	this->info = info;
	this->layer = 1;
	this->type = EntityType::GOO_BALL;

	if (!this->ball_template) {
		if (!GooBall::ball_templates.contains(this->info->typeEnum)) {
			throw std::runtime_error("GooBall::GooBall GooBallTemplate for GooBallType = " + std::to_string(static_cast<int>(this->info->typeEnum)) + " has not been loaded");
		}

		this->ball_template = &GooBall::ball_templates.at(this->info->typeEnum);
	}

	this->click_bounds = static_cast<EntityClickBoundShape*>(new EntityClickBoundCircle(0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * this->ball_template->bodyScale));

	GooBall::balls.insert({ this->info->uid, this });
}

GooBall::~GooBall() {
	delete this->click_bounds;
}

void GooBall::update() {
	this->position = this->info->pos;
}

// TODO: make this less awful
void GooBall::draw(sf::RenderWindow* window) {
	sf::Sprite sprite = *(*ResourceManager::getInstance()->getSpriteResource(this->ball_template->body_image_id))->get();

	// Set the origin to the center of the sprite
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	// we don't actually randomize the variance because this is an editor
	float scale = 0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * this->ball_template->bodyScale;
	sprite.setScale(sf::Vector2f(scale, scale));
	sprite.setPosition(Level::worldToScreen(this->position));

	if (this->selected) {
		sf::RectangleShape outline;

		// Set the size to match the scaled size of the sprite
		outline.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale));

		// Set the origin to the center of the rectangle
		outline.setOrigin(bounds.width * scale / 2.0f, bounds.height * scale / 2.0f);

		// Position the outline at the same location as the sprite
		outline.setPosition(sprite.getPosition());

		// Set the outline color and thickness
		outline.setOutlineColor(sf::Color::Blue); // Change to desired outline color
		outline.setOutlineThickness(2.0f); // Change to desired outline thickness

		// Set the fill color to transparent
		outline.setFillColor(sf::Color::Transparent);

		// Draw the outline
		window->draw(outline);
	}

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

GooBallInfo* GooBall::getInfo() {
	return this->info;
}

GooBallTemplate* GooBall::getTemplate() {
	return this->ball_template;
}

} // namespace gooforge