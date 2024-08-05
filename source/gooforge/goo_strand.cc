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

GooStrandState GooStrandState::deserialize(simdjson::ondemand::value json) {
	GooStrandState state;
	state.ball1UID = json.find_field("ball1UID").get_uint64().take_value();
	state.ball2UID = json.find_field("ball2UID").get_uint64().take_value();
	state.type = GooBallType(json.find_field("type").get_int64().take_value());
	state.filled = json.find_field("filled").get_bool().take_value();

	return state;
}

GooStrand::GooStrand(GooStrandState state) {
	this->state = state;
	this->z_index = 0;

	if (!this->ball_template) {
		if (GooBall::ball_templates.empty()) { // empty could signify a failed load, TODO: fix
			GooBall::loadGooBallTemplates("C:/Program Files/World of Goo 2/game/res/balls");
		}

		if (!GooBall::ball_templates.contains(this->state.type)) {
			throw std::runtime_error("GooStrand::GooStrand GooBallTemplate for GooBallType = " + std::to_string(static_cast<int>(this->state.type)) + " has not been loaded");
		}

		this->ball_template = &GooBall::ball_templates.at(this->state.type);
	}

	if (!GooBall::balls.contains(this->state.ball1UID)) {
		throw std::runtime_error("GooStrand::GooStrand GooBall with uid = " + std::to_string(static_cast<int>(this->state.ball1UID)) + " has not been loaded");
	}

	if (!GooBall::balls.contains(this->state.ball2UID)) {
		throw std::runtime_error("GooStrand::GooStrand GooBall with uid = " + std::to_string(static_cast<int>(this->state.ball1UID)) + " has not been loaded");
	}

	this->ball1 = GooBall::balls.at(this->state.ball1UID);
	this->ball2 = GooBall::balls.at(this->state.ball2UID);
}

void GooStrand::update() {
	this->position.x = (this->ball1->position.x + this->ball2->position.x) / 2.0;
	this->position.y = (this->ball1->position.y + this->ball2->position.y) / 2.0;
}

void GooStrand::draw(sf::RenderWindow* window) {
	sf::Sprite sprite = *(*ResourceManager::getInstance()->getSpriteResource(this->ball_template->strand_image_id))->get();

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
	float scale = distance / base_height_in_units;

	sprite.setScale(sf::Vector2f(base_scale, scale)); // Set x scale to match the distance, y scale is 1.0f for now

	Vector2f screen_position = this->position.scale(GOOFORGE_PIXELS_PER_UNIT);
	sprite.setPosition(sf::Vector2f(screen_position.x, -1.0 * screen_position.y));

	// Calculate angle between the two balls
	float angle = std::atan2(dy, dx) * 180.0 / 3.14; // Convert radians to degrees

	// Set the rotation of the sprite
	sprite.setRotation(angle + 90.0);

	// Draw the sprite
	window->draw(sprite);
}


} // namespace gooforge