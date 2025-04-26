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

ItemInstance::~ItemInstance() { delete this->click_bounds; }

std::expected<void, Error> ItemInstance::setup(ItemInstanceInfo info) {
    this->info = info;
    return this->refresh();
}

std::expected<void, Error> ItemInstance::refresh() {
    auto item_resource =
        ResourceManager::getInstance()->getResource<ItemResource>(
            "GOOFORGE_ITEM_RESOURCE_" + this->info.type);
    if (!item_resource) {
        return std::unexpected(item_resource.error());
    }

    auto info_file_result = item_resource.value()->get();
    if (!info_file_result) {
        return std::unexpected(info_file_result.error());
    }

    this->info_file = *info_file_result;

    size_t index = this->info.forcedRandomizationIndex != -1
                       ? this->info.forcedRandomizationIndex
                       : 0;
    this->object_info = &this->info_file->items[0].objects[index];

    auto sprite_resource =
        ResourceManager::getInstance()->getResource<SpriteResource>(
            this->object_info->name);
    if (!sprite_resource) {
        return std::unexpected(sprite_resource.error());
    }

    auto sprite = sprite_resource.value()->get();
    if (!sprite) {
        return std::unexpected(sprite.error());
    }

    this->display_sprite = *sprite;

    sf::Vector2u sprite_size_screen =
        this->display_sprite.getTexture()->getSize();
    Vector2f sprite_size_world =
        Level::screenToWorld(sf::Vector2f(sprite_size_screen));
    sprite_size_world.y *=
        -1.0f; // correct for flipped y-coordinate compensation
    float scale_x =
        this->info.scale.x * this->object_info->scale.x *
        (this->info.flipHorizontal != this->object_info->flipHorizontal ? -1.0f
                                                                        : 1.0f);
    float scale_y =
        this->info.scale.y * this->object_info->scale.y *
        (this->info.flipVertical != this->object_info->flipVertical ? -1.0f
                                                                    : 1.0f);

    sprite_size_world.x *= this->info.scale.x * this->object_info->scale.x;
    sprite_size_world.y *= this->info.scale.y * this->object_info->scale.y;
    if (this->click_bounds) delete this->click_bounds;
    this->click_bounds =
        static_cast<EntityClickBoundShape*>(new EntityClickBoundRectangle(
            sprite_size_world, this->object_info->pivot));

    return std::expected<void, Error>{};
}

void ItemInstance::update() {}

void ItemInstance::draw(sf::RenderWindow* window) {
    sf::FloatRect bounds = this->display_sprite.getLocalBounds();
    sf::Vector2f origin(
        this->object_info->pivot.x * bounds.width,
        bounds.height - (this->object_info->pivot.y * bounds.height));
    this->display_sprite.setOrigin(origin);

    float scale_x =
        this->info.scale.x * this->object_info->scale.x *
        (this->info.flipHorizontal != this->object_info->flipHorizontal ? -1.0f
                                                                        : 1.0f);
    float scale_y =
        this->info.scale.y * this->object_info->scale.y *
        (this->info.flipVertical != this->object_info->flipVertical ? -1.0f
                                                                    : 1.0f);
    this->display_sprite.setScale(scale_x, scale_y);
    this->display_sprite.setPosition(Level::worldToScreen(this->info.pos));
    this->display_sprite.setRotation(
        (this->info.rotation + this->object_info->rotation) *
        (-180.0f / std::numbers::pi));

    uint32_t argb = this->object_info->color;
    uint8_t alpha = (argb >> 24) & 0xFF;
    uint8_t red = (argb >> 16) & 0xFF;
    uint8_t green = (argb >> 8) & 0xFF;
    uint8_t blue = argb & 0xFF;

    this->display_sprite.setColor(sf::Color(red, green, blue, alpha));

    window->draw(this->display_sprite);

    /*
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
    }*/
}

sf::Sprite ItemInstance::getThumbnail() { return this->display_sprite; }

std::string ItemInstance::getDisplayName() {
    return "ItemInstance (" + this->info_file->items[0].name + ")";
}

Vector2f ItemInstance::getPosition() { return this->info.pos; }

float ItemInstance::getRotation() {
    return this->info.rotation + this->object_info->rotation;
}

float ItemInstance::getDepth() const { return this->info.depth; }

void ItemInstance::setPosition(Vector2f position) { this->info.pos = position; }

ItemInstanceInfo& ItemInstance::getInfo() { return this->info; }

// these names are not official
std::unordered_map<ItemType, std::string> ItemInstance::item_type_to_name = {
    {ItemType::INVALID, "Invalid"},
    {ItemType::GENERIC, "Generic"},
    {ItemType::PIPE_IN_LIQUID, "Pipe In Liquid"},
    {ItemType::CONNECTION, "Connection"},
    {ItemType::PIPE_IN, "Pipe In"},
    {ItemType::TILABLE, "Tilable"},
    {ItemType::GOD_RAY_AREA, "God Ray Area"},
    {ItemType::LINEAR_FORCE_FIELD, "Linear Force Field"},
    {ItemType::RADIAL_FORCE_FIELD, "Radial Force Field"},
    {ItemType::POOL, "Pool"},
    {ItemType::RANGE_END_CRITERIA, "Range End Criteria"},
    {ItemType::TORCH, "Torch"},
    {ItemType::TERRAIN_ATTRIB, "Terrain Attrib"},
    {ItemType::WATER, "Water"},
    {ItemType::SWING, "Swing"},
    {ItemType::FIREBALL, "Fireball"},
    {ItemType::GEAR, "Gear"},
    {ItemType::DEBRIS, "Debris"},
    {ItemType::ROPE, "Rope"},
    {ItemType::FIREWORKS_BOAT, "Fireworks Boat"},
    {ItemType::JELLY_CUBES_PIPE, "Jelly Cubes Pipe"},
    {ItemType::PISTON, "Piston"},
    {ItemType::DECORATION, "Decoration"},
    {ItemType::LEVEL_EXIT, "Level Exit"},
    {ItemType::TIMED_FADE, "Timed Fade"},
    {ItemType::SIGN_PAINTER_SIGN, "Sign Painter Sign"},
    {ItemType::TERRAIN_HIDE, "Terrain Hide"},
    {ItemType::TIMED_ANIMATION, "Timed Animation"},
    {ItemType::SELECT_ANIMATION, "Select Animation"},
    {ItemType::BUTTON, "Button"},
    {ItemType::DRAIN, "Drain"},
    {ItemType::WINCH, "Winch"},
    {ItemType::SQUIDDY, "Squiddy"},
    {ItemType::PIN_TO_JELLY, "Pin To Jelly"},
    {ItemType::BALL_DEBRIS, "Ball Debris"},
    {ItemType::RAIL_CONSTRAINT, "Rail Constraint"},
    {ItemType::SATELLITE_DISH, "Satellite Dish"},
    {ItemType::CAMERA_CONTROL, "Camera Control"},
    {ItemType::CAMERA_EOL, "Camera EOL"},
    {ItemType::ISLAND2, "Island 2"},
    {ItemType::PARTICLE_EFFECT_IN_LEVEL, "Particle Effect In Level"},
    {ItemType::EYE, "Eye"},
    {ItemType::AMBIENT_SOUND_AREA, "Ambient Sound Area"},
    {ItemType::AUTO_BOUNDS_AREA, "Auto Bounds Area"},
    {ItemType::ROCKET, "Rocket"},
    {ItemType::LEVEL_TEXT, "Level Text"},
    {ItemType::TENTACLE, "Tentacle"},
    {ItemType::OVERLAY, "Overlay"},
    {ItemType::OCD_FLAG, "OCD Flag"},
    {ItemType::ANGLER_HOOK, "Angler Hook"},
};

ItemType ItemInstance::getItemType() {
    return this->info_file->items[0].type; // NOT SAFE!!! todo: fix
}

std::vector<ItemUserVariableInfo> ItemInstance::getUserVariableInfo() {
    return this->info_file->items[0].userVariables; // NOT SAFE!!! todo: fix
}

std::vector<ItemInstanceUserVariableInfo>&
ItemInstance::getInstanceUserVariableInfo() {
    return this->info.userVariables;
}

std::string ItemInstance::getItemTemplateUUID() { return this->info.type; }

void ItemInstance::setItemTemplateUUID(std::string uuid) {
    this->info.type = uuid;
    this->refresh();
}

void ItemInstance::setDepth(float depth) { this->info.depth = depth; }

Vector2f ItemInstance::getScale() { return this->info.scale; }

void ItemInstance::setScale(Vector2f scale) {
    this->info.scale = scale;
    this->refresh();
}

void ItemInstance::setRotation(float rotation) {
    this->info.rotation = rotation;
}

int ItemInstance::getForcedRandomizationIndex() {
    return this->info.forcedRandomizationIndex;
}

void ItemInstance::setForcedRandomizationIndex(int index) {
    this->info.forcedRandomizationIndex = index;
}

} // namespace gooforge