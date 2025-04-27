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

#include <numbers>
#include <ranges>
#include <unordered_set>

#include "constants.hh"
#include "item.hh"
#include "level.hh"
#include "resource_manager.hh"

namespace gooforge {

std::expected<void, Error> TerrainGroup::setup(TerrainGroupInfo info) {
    this->info = info;

    return this->refresh();
}

std::expected<void, Error> TerrainGroup::refresh() {
    auto template_resource =
        ResourceManager::getInstance()->getResource<TerrainTemplatesResource>(
            "GOOFORGE_TERRAIN_TEMPLATES_RESOURCE");
    if (!template_resource) {
        return std::unexpected(template_resource.error());
    }

    auto template_info_file = template_resource.value()->get();
    if (!template_info_file) {
        return std::unexpected(template_info_file.error());
    }

    // TODO: handle not found case
    size_t index = 0;
    for (auto& terrain_template : (*template_info_file)->terrainTypes) {
        if (terrain_template.uuid == this->info.typeUuid) {
            this->template_info = &terrain_template;
            this->info.typeIndex = index; // this is kinda cursed
            break;
        }

        ++index;
    }

    auto sprite_resource =
        ResourceManager::getInstance()->getResource<SpriteResource>(
            this->template_info->baseSettings.image.imageId);
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

void TerrainGroup::update() {}

void TerrainGroup::draw(sf::RenderWindow* window) {
    // Create a render state with the texture
    sf::RenderStates states;
    sf::Texture texture = *this->display_sprite.getTexture();
    texture.setRepeated(true);
    states.texture = &texture;

    for (auto strand : this->terrain_strands) {
        auto locked_strand = strand;
        auto locked1 = locked_strand->ball1;
        auto locked2 = locked_strand->ball2;

        auto u = locked1->strands.size() < locked2->strands.size() ? locked1
                                                                   : locked2;
        auto v = u == locked1 ? locked2 : locked1;
        std::unordered_set<GooBall*> v_neighbors;
        for (auto v_strand : v->strands) {
            auto locked_v_strand = v_strand;
            auto locked_v1 = locked_v_strand->ball1;
            auto locked_v2 = locked_v_strand->ball2;

            auto neighbor = locked_v1 == v ? locked_v2 : locked_v1;
            v_neighbors.insert(neighbor);
        }
        for (auto w_strand : u->strands) {
            auto locked_w_strand = w_strand;
            auto locked_w1 = locked_w_strand->ball1;
            auto locked_w2 = locked_w_strand->ball2;

            auto w = locked_w1 == u ? locked_w2 : locked_w1;

            if (w != v && v_neighbors.contains(w)) {
                if (w->getTerrainGroup() != this) continue;

                sf::VertexArray tri(sf::PrimitiveType::Triangles, 3);

                // Set positions
                tri[0].position = Level::worldToScreen(u->info.pos);
                tri[1].position = Level::worldToScreen(v->info.pos);
                tri[2].position = Level::worldToScreen(w->info.pos);

                // Set texture coordinates based on world positions
                tri[0].texCoords = tri[0].position;
                tri[1].texCoords = tri[1].position;
                tri[2].texCoords = tri[2].position;

                // Set colors to white to ensure texture is visible with proper
                // coloring
                tri[0].color = sf::Color::White;
                tri[1].color = sf::Color::White;
                tri[2].color = sf::Color::White;

                // Draw with texture using render states
                window->draw(tri, states);
            }
        }
    }

    for (auto strand : this->terrain_strands) {
        auto locked_strand = strand;
        sf::VertexArray line(sf::Lines, 2);
        line[0].position =
            Level::worldToScreen(locked_strand->getBall1()->getPosition());
        line[0].color = sf::Color::Green;
        line[1].position =
            Level::worldToScreen(locked_strand->getBall2()->getPosition());
        line[1].color = sf::Color::Green;
        window->draw(line);
    }
}

std::string TerrainGroup::getDisplayName() {
    return "TerrainGroup (" + this->template_info->name + ")";
}

sf::Sprite TerrainGroup::getThumbnail() { return this->display_sprite; }

float TerrainGroup::getDepth() const { return this->info.depth; }

void TerrainGroup::setDepth(float depth) {
    this->info.depth = depth;
}

TerrainGroupInfo& TerrainGroup::getInfo() { return this->info; }

void TerrainGroup::notifyAddStrand(GooStrand* strand) {
    if (strand->getBall1()->getTerrainGroup() == this &&
        strand->getBall2()->getTerrainGroup() == this) {
        this->terrain_strands.insert(strand);
    }
}

void TerrainGroup::notifyRemoveStrand(GooStrand* strand) {
    this->terrain_strands.erase(strand);
}

void TerrainGroup::notifyUpdateStrand(GooStrand* strand) {
    if (this->terrain_strands.contains(strand)) {
        if (strand->getBall1()->getTerrainGroup() != this ||
            strand->getBall2()->getTerrainGroup() != this) {
            this->terrain_strands.erase(strand);
        }
    } else {
        if (strand->getBall1()->getTerrainGroup() == this &&
            strand->getBall2()->getTerrainGroup() == this) {
            this->terrain_strands.insert(strand);
        }
    }
}

std::string TerrainGroup::getTerrainTemplateUUID() {
    return this->info.typeUuid;
}

void TerrainGroup::setTerrainTemplateUUID(std::string uuid) {
    this->info.typeUuid = uuid;

    this->refresh();
}

int TerrainGroup::getSortOffset() const { return this->info.sortOffset; }

void TerrainGroup::setSortOffset(int offset) { this->info.sortOffset = offset; }

} // namespace gooforge