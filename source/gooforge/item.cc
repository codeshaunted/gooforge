// codeshaunted - gooforge
// source/gooforge/item.cc
// contains ItemInstance and related definitions
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

#include "item.hh"

#include <numbers>

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

ItemInstance::ItemInstance(ItemInstanceInfo* info) {
	this->info = info;
	this->layer = this->info->depth;

	Resource* resource = (*ResourceManager::getInstance()->getResource("GOOFORGE_ITEM_RESOURCE_" + this->info->type));
	this->info_file = *std::get<ItemResource>(*resource).get();
}

ItemInstance::~ItemInstance() {

}

void ItemInstance::update() {
	this->position = this->info->pos;
	this->rotation = this->info->rotation;
}

void ItemInstance::draw(sf::RenderWindow* window) {
	for (auto object : this->info_file->items[0].objects) {
		Resource* sprite_resource = (*ResourceManager::getInstance()->getResource(object.name));
		sf::Sprite sprite = *std::get<SpriteResource>(*sprite_resource).get();

		sf::FloatRect bounds = sprite.getLocalBounds();
		sf::Vector2f origin(object.pivot.x * bounds.width, bounds.height - (object.pivot.y * bounds.height));
		sprite.setOrigin(origin);

		sprite.setScale(sf::Vector2f(this->info->scale.x * object.scale.x, this->info->scale.y * object.scale.y));
		sprite.setPosition(Level::worldToScreen(this->position));
		sprite.rotate((this->rotation + object.rotation) * (-180.0f / std::numbers::pi));

		window->draw(sprite);
	}

	/*
	sf::RectangleShape outline(sf::Vector2f(bounds.width, bounds.height));
	outline.setOrigin(sprite.getOrigin());
	outline.setScale(sprite.getScale());
	outline.setPosition(sprite.getPosition());
	outline.setRotation(sprite.getRotation());

	// Set the outline color and thickness
	outline.setOutlineColor(sf::Color::Red);  // Choose your desired outline color
	outline.setOutlineThickness(1.0f);        // Adjust the thickness as needed
	outline.setFillColor(sf::Color::Transparent); // No fill

	// Draw the outline
	window->draw(outline);

	// Draw a circle at the origin
	sf::CircleShape originCircle(5.0f); // Radius of 5.0f
	originCircle.setFillColor(sf::Color::Blue); // Color the circle blue
	originCircle.setOrigin(5.0f, 5.0f); // Center the circle at its origin
	originCircle.setPosition(sprite.getPosition()); // Position it at the sprite's origin
	window->draw(originCircle);*/
}

} // namespace gooforge