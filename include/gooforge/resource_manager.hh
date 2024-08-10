// codeshaunted - gooforge
// include/gooforge/resource_manager.hh
// contains ResourceManager declarations
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

#ifndef GOOFORGE_RESOURCE_MANAGER_HH
#define GOOFORGE_RESOURCE_MANAGER_HH

#include <expected>
#include <filesystem>
#include <string_view>
#include <unordered_map>

#include "SFML/Graphics.hpp"

#include "error.hh"
#include "goo_ball.hh"
#include "item.hh"

namespace gooforge {

enum class ResourceType {
    SPRITE,
};

class BaseResource {
    public:
        BaseResource(std::string path) : path(path) {}
    protected:
        std::string path;
};

class SpriteResource : public BaseResource {
    public:
        SpriteResource(std::string path) : BaseResource(path), atlas_sprite_path("") {}
        SpriteResource(std::string atlas_sprite_path, sf::IntRect atlas_rect) : BaseResource(atlas_sprite_path), atlas_sprite_path(atlas_sprite_path), atlas_rect(atlas_rect) {}
        std::expected<sf::Sprite, Error> get();
    private:
        std::shared_ptr<sf::Texture> texture; // for some reason MSVC will not allow this to be a std::unique_ptr
        std::string atlas_sprite_path;
        sf::IntRect atlas_rect;
};

class BallTemplateResource : public BaseResource {
    public:
        BallTemplateResource(std::string path) : BaseResource(path) {}
        std::expected<BallTemplateInfo*, Error> get();
    private:
        std::shared_ptr<BallTemplateInfo> info; // for some reason MSVC will not allow this to be a std::unique_ptr
};

class ItemResource : public BaseResource {
    public:
        ItemResource(std::string path) : BaseResource(path) {}
        std::expected<ItemInfoFile*, Error> get();
    private:
        std::shared_ptr<ItemInfoFile> info_file; // for some reason MSVC will not allow this to be a std::unique_ptr
};

using Resource = std::variant<SpriteResource, BallTemplateResource, ItemResource>;

class ResourceManager {
    public:
        ~ResourceManager();
        static ResourceManager* getInstance();
        std::expected<void, Error> takeInventory(std::filesystem::path& base_path);
        std::expected<void, Error> loadResourceManifest(std::filesystem::path& path);
        std::expected<void, Error> loadAtlasManifest(std::filesystem::path& path);
        std::expected<Resource*, Error> getResource(std::string id);
    private:
        static ResourceManager* instance;
        std::filesystem::path base_path;
        std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
};

} // namespace gooforge

#endif // GOOFORGE_RESOURCE_MANAGER_HH