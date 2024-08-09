// codeshaunted - gooforge
// source/gooforge/resource_manager.cc
// contains ResourceManager definitions
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

#include "resource_manager.hh"

#include <fstream>

#include "pugixml.hpp"
#include "spdlog.h"

#include "boy_image.hh"
#include "buffer_stream.hh"

namespace gooforge {

BaseResource::~BaseResource() {
    if (this->asset) {
        delete this->asset;
    }
}

void BaseResource::unload() {
    if (this->asset) {
        delete this->asset;
    }
}

std::expected<sf::Sprite, Error> SpriteResource::get() {
    if (this->atlas_sprite) {
        const sf::Texture* texture = this->atlas_sprite->get()->getTexture();

        return sf::Sprite(*texture, this->atlas_rect);
    }

    if (this->asset) {
        sf::Texture* texture = static_cast<sf::Texture*>(this->asset);

        return sf::Sprite(*texture);
    } else {
        sf::Texture* texture = new sf::Texture();
        auto image = BoyImage::loadFromFile(this->path);
        if (!image) {
            return std::unexpected(image.error());
        }

        texture->loadFromImage(**image);

        this->asset = reinterpret_cast<void*>(texture);

        return this->get();
    }
}

ResourceManager::~ResourceManager() {
    for (auto resource : this->resources) {
        BaseResource* base_resource = std::visit([](auto& derived_resource) -> BaseResource* { return derived_resource; }, resource.second);
        delete base_resource;
    }
}

ResourceManager* ResourceManager::getInstance() {
    if (!ResourceManager::instance) {
        ResourceManager::instance = new ResourceManager();
        spdlog::info("Resource manager successfully initialized");
    }

    return ResourceManager::instance;
}

std::expected<void, Error> ResourceManager::takeInventory(std::filesystem::path& base_path) {
    this->base_path = base_path;

    // load manifests
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(base_path)) {
        auto path = entry.path();
        if (path.extension() == ".xml") {
            auto result = this->loadResourceManifest(path);
            if (!result) {
                return std::unexpected(result.error());
            }
        } else if (path.extension() == ".atlas") {
            auto result = this->loadAtlasManifest(path);
            if (!result) {
                return std::unexpected(result.error());
            }
        }
    }

    return std::expected<void, Error>{};
}

std::expected<void, Error> ResourceManager::loadResourceManifest(std::filesystem::path& path) {
    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_file(path.string().c_str());

    if (!result) {
        return std::unexpected(XMLDeserializeError(path.string(), result.description()));
    }

    pugi::xml_node root = document.child("ResourceManifest");

    for (auto resource = root.child("Resources"); resource; resource = resource.next_sibling("Resources")) {
        pugi::xml_node set_defaults = resource.child("SetDefaults");

        std::filesystem::path resource_path = this->base_path;
        std::string resource_id;
        if (set_defaults) {
            resource_path /= set_defaults.attribute("path").as_string();
            resource_id = set_defaults.attribute("id_prefix").as_string();
        }

        for (auto image = resource.child("Image"); image; image = image.next_sibling("Image")) {
            resource_id += image.attribute("id").as_string();
            resource_path /= image.attribute("path").as_string();
            resource_path.replace_extension(".image");

            SpriteResource* sprite_resource = new SpriteResource(resource_path.string());
            this->resources.insert({ resource_id, sprite_resource });
        }
    }

    return std::expected<void, Error>{};
}

std::expected<void, Error> ResourceManager::loadAtlasManifest(std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        return std::unexpected(FileOpenError(path.string()));
    }

    size_t size = file.tellg();
    file.seekg(0);

    char* data = new char[size];
    file.read(data, size);

    BufferStream stream(data, size);
    stream.seek(8); // skip header

    path.replace_extension("");
    SpriteResource* atlas_sprite = new SpriteResource(path.string()); // chop off .atlas
    this->resources.insert({ path.string(), atlas_sprite});
        
    uint32_t number_of_files = stream.read<uint32_t>();
    for (size_t i = 0; i < number_of_files; ++i) {
        std::string id;
        for (size_t j = 0; j < 64; ++j) {
            char character = stream.read<char>();

            if (character) {
                id += character;
            }
        }

        uint32_t x_offset = stream.read<uint32_t>();
        uint32_t y_offset = stream.read<uint32_t>();
        uint32_t x_size = stream.read<uint32_t>();
        uint32_t y_size = stream.read<uint32_t>();

        sf::IntRect rect(x_offset, y_offset, x_size, y_size);

        SpriteResource* sprite_resource = new SpriteResource(atlas_sprite, rect);
        this->resources.insert({id, sprite_resource});
    }

    return std::expected<void, Error>{};
}

std::expected<Resource, Error> ResourceManager::getResource(std::string id) {
    std::string id_string(id);
    if (!this->resources.contains(id_string)) {
        return std::unexpected(ResourceNotFoundError(id_string));
    }

    return this->resources.at(id_string);
}

ResourceManager* ResourceManager::instance = nullptr;

} // namespace gooforge