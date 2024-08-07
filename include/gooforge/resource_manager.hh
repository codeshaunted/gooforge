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
#include <string_view>
#include <unordered_map>

#include "SFML/Graphics.hpp"

#include "error.hh"

namespace gooforge {

enum class ResourceType {
    SPRITE,
};

class Resource {
    public:
        Resource(ResourceType type, std::string_view path) : type(type), path(path), asset(nullptr) {}
        ~Resource();
        void unload();
    protected:
        ResourceType type;
        std::string path;
        void* asset;
    
    friend class ResourceManager;
};

class SpriteResource : public Resource {
    public:
        SpriteResource(std::string_view path) : Resource(ResourceType::SPRITE, path), atlas_sprite(nullptr) {}
        SpriteResource(SpriteResource* atlas_sprite, sf::IntRect atlas_rect) : Resource(ResourceType::SPRITE, ""), atlas_sprite(atlas_sprite), atlas_rect(atlas_rect) {}
        std::expected<sf::Sprite, LegacyError> get();
    private:
        SpriteResource* atlas_sprite;
        sf::IntRect atlas_rect;
};

class ResourceManager {
    public:
        ~ResourceManager();
        static ResourceManager* getInstance();
        std::expected<void, Error> loadManifest(std::string_view path);
        std::expected<SpriteResource*, Error> getSpriteResource(std::string_view id);
    private:
        static ResourceManager* instance;
        std::unordered_map<std::string, Resource*> resources;
};

} // namespace gooforge

#endif // GOOFORGE_RESOURCE_MANAGER_HH