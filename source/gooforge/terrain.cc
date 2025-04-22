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
#include <unordered_set>

#include "constants.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

std::expected<void, Error> TerrainGroup::setup(TerrainGroupInfo info) {
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
        if (terrain_template.uuid == this->info.typeUuid) {
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
    // Create a render state with the texture
    sf::RenderStates states;
    sf::Texture texture = *this->display_sprite.getTexture();
    texture.setRepeated(true);
    states.texture = &texture;
    
    for (auto strand : this->terrain_strands) {
        GooBall* u = strand->ball1->strands.size() < strand->ball2->strands.size() ? strand->ball1 : strand->ball2;
        GooBall* v = u == strand->ball1 ? strand->ball2 : strand->ball1;
        std::unordered_set<GooBall*> v_neighbors;
        for (auto v_strand : v->strands) {
            GooBall* neighbor = v_strand->ball1 == v ? v_strand->ball2 : v_strand->ball1;
            v_neighbors.insert(neighbor);
        }
        for (auto w_strand : u->strands) {
            GooBall* w = w_strand->ball1 == u ? w_strand->ball2 : w_strand->ball1;
           
            if (w != v && v_neighbors.contains(w)) {
                sf::VertexArray tri(sf::PrimitiveType::Triangles, 3);

                // Set positions
                tri[0].position = Level::worldToScreen(u->info.pos);
                tri[1].position = Level::worldToScreen(v->info.pos);
                tri[2].position = Level::worldToScreen(w->info.pos);
                
                // Set texture coordinates based on world positions
                tri[0].texCoords = tri[0].position;
                tri[1].texCoords = tri[1].position;
                tri[2].texCoords = tri[2].position;

                // Set colors to white to ensure texture is visible with proper coloring
                tri[0].color = sf::Color::White;
                tri[1].color = sf::Color::White;
                tri[2].color = sf::Color::White;
                
                // Draw with texture using render states
                window->draw(tri, states);
            }
        }
    }

    for (auto strand : this->terrain_strands) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = Level::worldToScreen(strand->ball1->info.pos);
        line[0].color = sf::Color::Green;
        line[1].position = Level::worldToScreen(strand->ball2->info.pos);
        line[1].color = sf::Color::Green;
        window->draw(line);
    }
}

void TerrainGroup::addTerrainBall(GooBall* goo_ball) {
    this->terrain_balls.push_back(goo_ball);
}

void TerrainGroup::addTerrainStrand(GooStrand* goo_strand) {
    this->terrain_strands.insert(goo_strand);
}

std::string TerrainGroup::getDisplayName() {
	return "TerrainGroup (" + this->template_info->name + ")";
}

sf::Sprite TerrainGroup::getThumbnail() {
    return this->display_sprite;
}

float TerrainGroup::getDepth() const {
    return this->info.depth;
}

TerrainGroupInfo& TerrainGroup::getInfo() {
    return this->info;
}

} // namespace gooforge