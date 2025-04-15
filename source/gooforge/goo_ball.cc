// codeshaunted - gooforge
// source/gooforge/goo_ball.cc
// contains GooBall definitions
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

#include "goo_ball.hh"

#include <filesystem>

#include "spdlog.h"

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

GooBall::~GooBall() {
	delete this->click_bounds;
}

std::expected<void, Error> GooBall::setup(GooBallInfo info) {
	this->info = info;
	return this->refresh();
}

std::expected<void, Error> GooBall::refresh() {
	std::string resource_id = "GOOFORGE_BALL_TEMPLATE_RESOURCE_" + std::to_string(static_cast<int>(this->info.typeEnum));
	auto template_resource = ResourceManager::getInstance()->getResource<BallTemplateResource>(resource_id);
	if (!template_resource) {
		return std::unexpected(template_resource.error());
	}

	auto template_info = template_resource.value()->get();
	if (!template_info) {
		return std::unexpected(template_info.error());
	}

	this->ball_template = *template_info;

	bool found_body_part = false;
	for (auto& part : this->ball_template->ballParts) {
		if (part.name == this->ball_template->bodyPart.partName) {
			std::string sprite_resource_id;
			// hardcoded cases for balls that require flash animations
			// TODO: implement flash
			if (part.images.empty()) {
				if (this->info.typeEnum == GooBallType::THRUSTER) {
					sprite_resource_id = "FlashAnim_BallThruster_body";
				}
				else if (this->info.typeEnum == GooBallType::LAUNCHER_L2L) {
					sprite_resource_id = "FlashAnim_LiquidLauncher_body";
				}
				else if (this->info.typeEnum == GooBallType::LAUNCHER_L2B) {
					sprite_resource_id = "FlashAnim_BallLauncher_body";
				}
				else if (this->info.typeEnum == GooBallType::LIGHTBALL) {
					sprite_resource_id = "FlashAnim_Lightball_ball";
				}
				else {
					return std::unexpected(GooBallSetupError(this->info.uid, "failed to find body part image"));
				}
			}
			else {
				sprite_resource_id = part.images[0].imageId.imageId;
			}

			auto sprite_resource = ResourceManager::getInstance()->getResource<SpriteResource>(sprite_resource_id);
			if (!sprite_resource) {
				return std::unexpected(sprite_resource.error());
			}
			
			auto sprite = sprite_resource.value()->get();
			if (!sprite) {
				return std::unexpected(sprite.error());
			}

			this->display_sprite = *sprite;

			this->click_bounds = static_cast<EntityClickBoundShape*>(new EntityClickBoundCircle(0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * part.scale));
			this->body_part = &part;
			found_body_part = true;
			break;
		}
	}
	
	if (!found_body_part) {
		return std::unexpected(GooBallSetupError(this->info.uid, "failed to find specified body part"));
	}

	return std::expected<void, Error>{};
}

void GooBall::update() {
}

// TODO: make this less awful
void GooBall::draw(sf::RenderWindow* window) {
	sf::FloatRect bounds = this->display_sprite.getLocalBounds();
	this->display_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	// we don't actually randomize the variance because this is an editor
	float scale = 0.5 * this->ball_template->width * (1.0 + this->ball_template->sizeVariance) * this->body_part->scale;
	this->display_sprite.setScale(sf::Vector2f(scale, scale));
	this->display_sprite.setPosition(Level::worldToScreen(this->info.pos));
	this->display_sprite.setRotation(-1.0f * Level::radiansToDegrees(this->info.angle));

	window->draw(this->display_sprite);

	if (this->selected) {
		sf::RectangleShape outline;

		// Set the size to match the scaled size of the sprite
		outline.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale));

		// Set the origin to the center of the rectangle
		outline.setOrigin(bounds.width * scale / 2.0f, bounds.height * scale / 2.0f);

		// Position the outline at the same location as the sprite
		outline.setPosition(this->display_sprite.getPosition());

		// Set the outline color and thickness
		outline.setOutlineColor(sf::Color::Blue); // Change to desired outline color
		outline.setOutlineThickness(2.0f); // Change to desired outline thickness

		// Set the fill color to transparent
		outline.setFillColor(sf::Color::Transparent);

		// Draw the outline
		window->draw(outline);
	}
}

sf::Sprite GooBall::getThumbnail() {
	return this->display_sprite;
}

std::string GooBall::getDisplayName() {
	return "GooBall (" + GooBall::ball_type_to_name.at(this->info.typeEnum) + ")";
}

Vector2f GooBall::getPosition() {
	return this->info.pos;
}

float GooBall::getRotation() {
	return this->info.angle;
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
	{"Fireworks", GooBallType::FIREWORKS},
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

std::unordered_map<GooBallType, std::string> GooBall::ball_type_to_name = {
	{GooBallType::INVALID, "Invalid"},
	{GooBallType::COMMON, "Common"},
	{GooBallType::COMMON_ALBINO, "CommonAlbino"},
	{GooBallType::IVY, "Ivy"},
	{GooBallType::BALLOON, "Balloon"},
	{GooBallType::GOOLF_SINGLE, "GoolfSingle"},
	{GooBallType::ANCHOR, "Anchor"},
	{GooBallType::LAUNCHER_L2B, "LauncherL2B"},
	{GooBallType::GOO_PRODUCT, "GooProduct"},
	{GooBallType::THRUSTER, "Thruster"},
	{GooBallType::TERRAIN, "Terrain"},
	{GooBallType::BALLOON_EYE, "BalloonEye"},
	{GooBallType::CONDUIT, "Conduit"},
	{GooBallType::LAUNCHER_L2L, "LauncherL2L"},
	{GooBallType::GOO_PRODUCT_WHITE, "GooProductWhite"},
	{GooBallType::GROW, "Grow"},
	{GooBallType::BOMB_STICKY, "BombSticky"},
	{GooBallType::ROPE, "Rope"},
	{GooBallType::BOUNCY, "Bouncy"},
	{GooBallType::FISH, "Fish"},
	{GooBallType::TIME_BUG, "TimeBug"},
	{GooBallType::MATCH_STICK, "MatchStick"},
	{GooBallType::COMMON_WORLD, "CommonWorld"},
	{GooBallType::FIREWORKS, "Fireworks"},
	{GooBallType::LIGHTBALL, "Lightball"},
	{GooBallType::TWU_BIT, "TwuBit"},
	{GooBallType::TWU_BIT_BIT, "TwuBitBit"},
	{GooBallType::ADAPTER, "Adapter"},
	{GooBallType::WINCH, "Winch"},
	{GooBallType::SHRINK, "Shrink"},
	{GooBallType::JELLY, "Jelly"},
	{GooBallType::GOOLF, "Goolf"},
	{GooBallType::THIS_WAY_UP, "ThisWayUp"},
	{GooBallType::LIQUID_LEVEL_EXIT, "LiquidLevelExit"},
	{GooBallType::EYE, "Eye"},
	{GooBallType::UTIL_ATTACH_WALKABLE, "UtilAttachWalkable"}
};

GooBallInfo& GooBall::getInfo() {
	return this->info;
}

BallTemplateInfo* GooBall::getTemplate() {
	return this->ball_template;
}

void GooBall::addStrand(GooStrand* strand) {
	this->strands.insert(strand);
}

} // namespace gooforge