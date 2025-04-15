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

std::expected<void, Error> ItemInstance::setup(ItemInstanceInfo info) {
	this->info = info;
	return this->refresh();
}

std::expected<void, Error> ItemInstance::refresh() {
	auto item_resource = ResourceManager::getInstance()->getResource<ItemResource>("GOOFORGE_ITEM_RESOURCE_" + this->info.type);
	if (!item_resource) {
		return std::unexpected(item_resource.error());
	}

	auto info_file_result = item_resource.value()->get();
	if (!info_file_result) {
		return std::unexpected(info_file_result.error());
	}

	this->info_file = *info_file_result;
	
	size_t index = this->info.forcedRandomizationIndex != -1 ? this->info.forcedRandomizationIndex : 0;
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

	sf::Vector2u sprite_size_screen = this->display_sprite.getTexture()->getSize();
	Vector2f sprite_size_world = Level::screenToWorld(sf::Vector2f(sprite_size_screen));
	sprite_size_world.y *= -1.0f; // correct for flipped y-coordinate compensation
	float scale_x = this->info.scale.x * this->object_info->scale.x * (this->info.flipHorizontal != this->object_info->flipHorizontal ? -1.0f : 1.0f);
	float scale_y = this->info.scale.y * this->object_info->scale.y * (this->info.flipVertical != this->object_info->flipVertical ? -1.0f : 1.0f);
	sprite_size_world.x *= scale_x;
	sprite_size_world.y *= scale_y;
	this->click_bounds = static_cast<EntityClickBoundShape*>(new EntityClickBoundRectangle(sprite_size_world, this->object_info->pivot));

	return std::expected<void, Error>{};
}

void ItemInstance::update() {
}

void ItemInstance::draw(sf::RenderWindow* window) {
	sf::FloatRect bounds = this->display_sprite.getLocalBounds();
	sf::Vector2f origin(this->object_info->pivot.x * bounds.width, bounds.height - (this->object_info->pivot.y * bounds.height));
	this->display_sprite.setOrigin(origin);

	float scale_x = this->info.scale.x * this->object_info->scale.x * (this->info.flipHorizontal != this->object_info->flipHorizontal ? -1.0f : 1.0f);
	float scale_y = this->info.scale.y * this->object_info->scale.y * (this->info.flipVertical != this->object_info->flipVertical ? -1.0f : 1.0f);
	this->display_sprite.setScale(scale_x, scale_y);
	this->display_sprite.setPosition(Level::worldToScreen(this->info.pos));
	this->display_sprite.setRotation((this->info.rotation + this->object_info->rotation) * (-180.0f / std::numbers::pi));

	uint32_t argb = this->object_info->color;
	uint8_t alpha = (argb >> 24) & 0xFF;
	uint8_t red = (argb >> 16) & 0xFF;
	uint8_t green = (argb >> 8) & 0xFF;
	uint8_t blue = argb & 0xFF;

	this->display_sprite.setColor(sf::Color(red, green, blue, alpha));

	window->draw(this->display_sprite);

	if (this->selected) {
		sf::FloatRect bounds = this->display_sprite.getLocalBounds();

		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(bounds.width, bounds.height));
		rect.setOrigin(this->display_sprite.getOrigin());
		rect.setScale(this->display_sprite.getScale());
		rect.setPosition(this->display_sprite.getPosition());
		rect.setRotation(this->display_sprite.getRotation());
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(sf::Color::Blue);
		rect.setOutlineThickness(2.0f);

		window->draw(rect);
	}
}

sf::Sprite ItemInstance::getThumbnail() {
	return this->display_sprite;
}

std::string ItemInstance::getDisplayName() {
	return "ItemInstance (" + this->info_file->items[0].name + ")";
}

Vector2f ItemInstance::getPosition() {
	return this->info.pos;
}

float ItemInstance::getRotation() {
	return this->info.rotation + this->object_info->rotation;
}

float ItemInstance::getDepth() const {
	return this->info.depth;
}

void ItemInstance::setPosition(Vector2f position) {
	this->info.pos = position;
}

ItemInstanceInfo& ItemInstance::getInfo() {
	return this->info;
}

} // namespace gooforge