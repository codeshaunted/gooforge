// codeshaunted - gooforge
// include/gooforge/resource_manager.hh
// contains ResourceManager declarations
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
#include "terrain.hh"

namespace gooforge {

enum class ResourceType {
    SPRITE,
};

class BaseResource {
    public:
        BaseResource(std::string path) : path(path) {}
        virtual ~BaseResource();
        virtual void unload() {}

    protected:
        std::string path;
};

class SpriteResource : public BaseResource {
    public:
        SpriteResource(std::string path) : BaseResource(path) {}
        SpriteResource(std::string atlas_sprite_path, sf::IntRect atlas_rect)
            : BaseResource(atlas_sprite_path),
              atlas_sprite(true),
              atlas_rect(atlas_rect) {}
        std::expected<sf::Sprite, Error> get();
        void unload() override;

    private:
        sf::Texture* texture = nullptr;
        bool atlas_sprite = false;
        sf::IntRect atlas_rect;
};

class BallTemplateResource : public BaseResource {
    public:
        BallTemplateResource(std::string path) : BaseResource(path) {}
        std::expected<BallTemplateInfo*, Error> get();
        void unload() override;

    private:
        BallTemplateInfo* info = nullptr;
};

class ItemResource : public BaseResource {
    public:
        ItemResource(std::string path) : BaseResource(path) {}
        std::expected<ItemInfoFile*, Error> get();
        void unload() override;

    private:
        ItemInfoFile* info_file = nullptr;
};

class TerrainTemplatesResource : public BaseResource {
    public:
        TerrainTemplatesResource(std::string path) : BaseResource(path) {}
        std::expected<TerrainTemplateInfoFile*, Error> get();
        void unload() override;

    private:
        TerrainTemplateInfoFile* info_file = nullptr;
};

using Resource = std::variant<SpriteResource, BallTemplateResource,
                              ItemResource, TerrainTemplatesResource>;

class ResourceManager {
    public:
        ~ResourceManager();
        static ResourceManager* getInstance();
        std::expected<void, Error> takeInventory(
            std::filesystem::path& base_path);
        std::expected<void, Error> loadResourceManifest(
            std::filesystem::path& path);
        std::expected<void, Error> loadAtlasManifest(
            std::filesystem::path& path);
        template <typename T>
        std::expected<T*, Error> getResource(std::string id);
        void unloadAll();

    private:
        static ResourceManager* instance;
        std::filesystem::path base_path;
        std::unordered_map<std::string, Resource*> resources;
};

template <typename T>
std::expected<T*, Error> ResourceManager::getResource(std::string id) {
    if (!this->resources.contains(id)) {
        return std::unexpected(ResourceNotFoundError(id));
    }

    Resource* resource = this->resources.at(id);

    if (T* t_resource = std::get_if<T>(resource)) {
        return t_resource;
    }
}

} // namespace gooforge

#endif // GOOFORGE_RESOURCE_MANAGER_HH