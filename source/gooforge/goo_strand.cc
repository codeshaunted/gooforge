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
#include "resource_manager.hh"

namespace gooforge {

GooStrand::~GooStrand() {

}

void GooStrand::update() {
	/*
	this->position.x = (this->ball1->position.x + this->ball2->position.x) / 2.0;
	this->position.y = (this->ball1->position.y + this->ball2->position.y) / 2.0;*/
}

// TODO: make this less awful
void GooStrand::draw(sf::RenderWindow* window) {/*
	Resource sprite_resource = (*ResourceManager::getInstance()->getResource(this->ball_template->strand_image_id));
	sf::Sprite sprite = *std::get<SpriteResource*>(sprite_resource)->get();

	// Set the origin to the center of the sprite
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	// Calculate the distance between the two balls
	float dx = this->ball2->position.x - this->ball1->position.x;
	float dy = (this->ball2->position.y - this->ball1->position.y) * -1.0;
	float distance = std::sqrt((dx * dx) + (dy * dy));

	// Set the scale based on the distance
	float base_scale = 0.5 * this->ball_template->width;
	float base_height_in_units = (bounds.height / GOOFORGE_PIXELS_PER_UNIT);
	float base_width_in_units = (bounds.width / GOOFORGE_PIXELS_PER_UNIT);
	float height_scale = distance / base_height_in_units;
	float width_scale = this->ball_template->strandThickness / base_width_in_units;

	sprite.setScale(sf::Vector2f(width_scale, height_scale)); // Set x scale to match the distance, y scale is 1.0f for now

	Vector2f screen_position = this->position.scale(GOOFORGE_PIXELS_PER_UNIT);
	sprite.setPosition(sf::Vector2f(screen_position.x, -1.0 * screen_position.y));

	// Calculate angle between the two balls
	float angle = std::atan2(dy, dx) * 180.0 / 3.14; // Convert radians to degrees

	// Set the rotation of the sprite
	sprite.setRotation(angle + 90.0);

	// Draw the sprite
	window->draw(sprite);*/
}


} // namespace gooforge