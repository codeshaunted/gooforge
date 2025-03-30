// codeshaunted - gooforge
// source/gooforge/goo_strand.cc
// contains GooStrand definitions
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

#include "goo_strand.hh"

#include <filesystem>

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

GooStrand::~GooStrand() {

}

std::expected<void, Error> GooStrand::setup(GooStrandInfo* info, GooBall* ball1, GooBall* ball2) {
	this->info = info;
	this->ball1 = ball1;
	this->ball2 = ball2;

	return this->refresh();
}

std::expected<void, Error> GooStrand::refresh() {
	this->depth = -std::numeric_limits<float>::max() + 2;

	std::string resource_id = "GOOFORGE_BALL_TEMPLATE_RESOURCE_" + std::to_string(static_cast<int>(this->info->type));
	auto template_resource = ResourceManager::getInstance()->getResource<BallTemplateResource>(resource_id);
	if (!template_resource) {
		return std::unexpected(template_resource.error());
	}

	auto template_info = template_resource.value()->get();
	if (!template_info) {
		return std::unexpected(template_info.error());
	}

	this->ball_template = *template_info;

	auto sprite_resource = ResourceManager::getInstance()->getResource<SpriteResource>(this->ball_template->strandImageId.imageId);
	if (!sprite_resource) {
		return std::unexpected(sprite_resource.error());
	}
	
	auto sprite = sprite_resource.value()->get();
	if (!sprite) {
		return std::unexpected(sprite.error());
	}

	this->display_sprite = *sprite;

	return std::expected<void, Error>{};
}

void GooStrand::update() {
}

// TODO: make this less awful
void GooStrand::draw(sf::RenderWindow* window) {
	/*
	// Create a line using the positions of the two balls
    sf::Vertex line[] =
    {
        sf::Vertex(Level::worldToScreen(this->ball1->getPosition())),
        sf::Vertex(Level::worldToScreen(this->ball2->getPosition()))
    };

    // Set the line's color (you can change this)
    line[0].color = sf::Color::Green;
    line[1].color = sf::Color::Green;

    // Draw the line
    window->draw(line, 2, sf::Lines);*/

	// Set the origin to the center of the sprite
	sf::FloatRect bounds = this->display_sprite.getLocalBounds();
	this->display_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	// Calculate the distance between the two balls
	float dx = this->ball2->info->pos.x - this->ball1->info->pos.x;
	float dy = (this->ball2->info->pos.y - this->ball1->info->pos.y) * -1.0;
	float distance = std::sqrt((dx * dx) + (dy * dy));

	// Set the scale based on the distance
	float base_scale = 0.5 * this->ball_template->width;
	float base_height_in_units = (bounds.height / GOOFORGE_PIXELS_PER_UNIT);
	float base_width_in_units = (bounds.width / GOOFORGE_PIXELS_PER_UNIT);
	float height_scale = distance / base_height_in_units;
	float width_scale = this->ball_template->strandThickness / base_width_in_units;

	this->display_sprite.setScale(sf::Vector2f(width_scale, height_scale)); // Set x scale to match the distance, y scale is 1.0f for now

	auto screen_position = Level::worldToScreen(Vector2f((this->ball1->info->pos.x + this->ball2->info->pos.x) / 2.0, (this->ball1->info->pos.y + this->ball2->info->pos.y) / 2.0));
	this->display_sprite.setPosition(screen_position);

	// Calculate angle between the two balls
	float angle = Level::radiansToDegrees(std::atan2(dy, dx));

	// Set the rotation of the sprite
	this->display_sprite.setRotation(angle + 90.0);

	// Draw the sprite
	window->draw(this->display_sprite);
}

sf::Sprite GooStrand::getThumbnail() {
	return this->display_sprite;
}

std::string GooStrand::getDisplayName() {
	return "GooStrand (" + GooBall::ball_type_to_name.at(this->info->type) + ")";
}

} // namespace gooforge