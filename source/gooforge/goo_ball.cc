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

GooBall::GooBall(GooBallInfo* info) {
	this->info = info;
	this->layer = 1;
	this->type = EntityType::GOO_BALL;

	if (!this->ball_template) {
		Resource template_resource = *ResourceManager::getInstance()->getResource("GOOFORGE_BALL_TEMPLATE_RESOURCE_" + std::to_string(static_cast<int>(this->info->typeEnum)));
		
		this->ball_template = *std::get<BallTemplateResource*>(template_resource)->get();
	}

	this->click_bounds = static_cast<EntityClickBoundShape*>(new EntityClickBoundCircle(0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * this->ball_template->ballParts[0].scale));

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
	Resource sprite_resource = (*ResourceManager::getInstance()->getResource(this->ball_template->ballParts[0].images[0].imageId.imageId));
	sf::Sprite sprite = *std::get<SpriteResource*>(sprite_resource)->get();

	// Set the origin to the center of the sprite
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	// we don't actually randomize the variance because this is an editor
	float scale = 0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * this->ball_template->ballParts[0].scale;
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

std::unordered_map<unsigned int, GooBall*> GooBall::balls;

GooBallInfo* GooBall::getInfo() {
	return this->info;
}

BallTemplateInfo* GooBall::getTemplate() {
	return this->ball_template;
}

} // namespace gooforge