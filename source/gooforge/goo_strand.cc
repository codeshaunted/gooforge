// codeshaunted - gooforge
// source/gooforge/goo_strand.cc
// contains GooStrand definitions
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

#include "goo_strand.hh"

#include <filesystem>

#include "constants.hh"
#include "resource_manager.hh"

namespace gooforge {

GooStrand::GooStrand(GooStrandInfo* info) {
	this->info = info;
	this->layer = 0;
	this->type = EntityType::GOO_STRAND;
	/*

	if (!this->ball_template) {
		if (!GooBall::ball_templates.contains(this->info->type)) {
			throw std::runtime_error("GooStrand::GooStrand GooBallTemplate for GooBallType = " + std::to_string(static_cast<int>(this->info->type)) + " has not been loaded");
		}

		this->ball_template = &GooBall::ball_templates.at(this->info->type);
	}

	if (!GooBall::balls.contains(this->info->ball1UID)) {
		throw std::runtime_error("GooStrand::GooStrand GooBall with uid = " + std::to_string(static_cast<int>(this->info->ball1UID)) + " has not been loaded");
	}

	if (!GooBall::balls.contains(this->info->ball2UID)) {
		throw std::runtime_error("GooStrand::GooStrand GooBall with uid = " + std::to_string(static_cast<int>(this->info->ball1UID)) + " has not been loaded");
	}

	this->ball1 = GooBall::balls.at(this->info->ball1UID);
	this->ball2 = GooBall::balls.at(this->info->ball2UID);*/
}

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