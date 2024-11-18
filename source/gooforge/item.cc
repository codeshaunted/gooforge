// codeshaunted - gooforge
// source/gooforge/item.cc
// contains ItemInstance and related definitions
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

#include "item.hh"

#include <numbers>
#include <ranges>

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

ItemInstance::~ItemInstance() {

}

std::expected<void, Error> ItemInstance::setup(ItemInstanceInfo* info) {
	this->info = info;
	return this->refresh();
}

std::expected<void, Error> ItemInstance::refresh() {
	auto item_resource = ResourceManager::getInstance()->getResource<ItemResource>("GOOFORGE_ITEM_RESOURCE_" + this->info->type);
	if (!item_resource) {
		return std::unexpected(item_resource.error());
	}

	auto info_file_result = item_resource.value()->get();
	if (!info_file_result) {
		return std::unexpected(info_file_result.error());
	}

	this->info_file = *info_file_result;
	
	size_t index = this->info->forcedRandomizationIndex != -1 ? this->info->forcedRandomizationIndex : 0;
	this->object_info = &this->info_file->items[0].objects[index];

	auto sprite_resource = ResourceManager::getInstance()->getResource<SpriteResource>(this->object_info->name);
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

void ItemInstance::update() {
	this->position = this->info->pos;
	this->rotation = this->info->rotation;
}

void ItemInstance::draw(sf::RenderWindow* window) {
	sf::FloatRect bounds = this->display_sprite.getLocalBounds();
	sf::Vector2f origin(this->object_info->pivot.x * bounds.width, bounds.height - (this->object_info->pivot.y * bounds.height));
	this->display_sprite.setOrigin(origin);

	float scale_x = this->info->scale.x * this->object_info->scale.x * (this->info->flipHorizontal != this->object_info->flipHorizontal ? -1.0f : 1.0f);
	float scale_y = this->info->scale.y * this->object_info->scale.y * (this->info->flipVertical != this->object_info->flipVertical ? -1.0f : 1.0f);
	this->display_sprite.setScale(scale_x, scale_y);
	this->display_sprite.setPosition(Level::worldToScreen(this->position));
	this->display_sprite.setRotation((this->rotation + this->object_info->rotation) * (-180.0f / std::numbers::pi));

	uint32_t argb = this->object_info->color;
	uint8_t alpha = (argb >> 24) & 0xFF;
	uint8_t red = (argb >> 16) & 0xFF;
	uint8_t green = (argb >> 8) & 0xFF;
	uint8_t blue = argb & 0xFF;

	this->display_sprite.setColor(sf::Color(red, green, blue, alpha));

	window->draw(this->display_sprite);

	if (this->selected) {
		sf::RectangleShape outline;

		// Set the size to match the scaled size of the sprite
		outline.setSize(sf::Vector2f(bounds.width * scale_x, bounds.height * scale_y));

		// Set the origin to the center of the rectangle
		outline.setOrigin(bounds.width * scale_x / 2.0f, bounds.height * scale_y / 2.0f);

		// Position the outline at the same location as the sprite
		outline.setPosition(this->display_sprite.getPosition());
		outline.setRotation(this->display_sprite.getRotation());

		// Set the outline color and thickness
		outline.setOutlineColor(sf::Color::Blue); // Change to desired outline color
		outline.setOutlineThickness(2.0f); // Change to desired outline thickness

		// Set the fill color to transparent
		outline.setFillColor(sf::Color::Transparent);

		// Draw the outline
		window->draw(outline);
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

sf::Sprite ItemInstance::getThumbnail() {
	return this->display_sprite;
}

std::string ItemInstance::getDisplayName() {
	return "ItemInstance (" + this->info_file->items[0].name + ")";
}

ItemInstanceInfo* ItemInstance::getInfo() {
	return this->info;
}

} // namespace gooforge