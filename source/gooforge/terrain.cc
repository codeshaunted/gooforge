// codeshaunted - gooforge
// source/gooforge/terrain.cc
// contains Terrain and related definitions
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

std::expected<void, Error> TerrainGroup::setup(TerrainGroupInfo* info) {
    this->info = info;

    return this->refresh();
}

std::expected<void, Error> TerrainGroup::refresh() {
    auto template_resource = ResourceManager::getInstance()->getResource<TerrainTemplatesResource>("GOOFORGE_TERRAIN_TEMPLATES_RESOURCE");
	if (!template_resource) {
		return std::unexpected(template_resource.error());
	}

	auto template_info_file = template_resource.value()->get();
	if (!template_info_file) {
		return std::unexpected(template_info_file.error());
	}

    // TODO: handle not found case
    for (auto& terrain_template : (*template_info_file)->terrainTypes) {
        if (terrain_template.uuid == this->info->typeUuid) {
            this->template_info = &terrain_template;
            break;
        }
    }

    auto sprite_resource = ResourceManager::getInstance()->getResource<SpriteResource>(this->template_info->baseSettings.image.imageId);
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

void TerrainGroup::update() {

}

void TerrainGroup::draw(sf::RenderWindow* window) {
    for (auto goo_ball : this->terrain_balls) {
        
        sf::CircleShape circle(10); // Radius of 10 for the circle
        circle.setFillColor(sf::Color::Red); // Set the circle color to red
        circle.setPosition(Level::worldToScreen(goo_ball->getPosition())); // Set the position of the circle to goo_ball's position
        
        window->draw(circle); // Draw the circle on the window
    }
}


void TerrainGroup::addTerrainBall(GooBall* goo_ball) {
    this->terrain_balls.push_back(goo_ball);
}

std::string TerrainGroup::getDisplayName() {
	return "TerrainGroup (" + this->template_info->name + ")";
}

sf::Sprite TerrainGroup::getThumbnail() {
    return this->display_sprite;
}

} // namespace gooforge