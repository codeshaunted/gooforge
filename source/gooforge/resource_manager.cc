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

#include "boy_image.hh"
#include "buffer_stream.hh"

namespace gooforge {

Resource::~Resource() {
    if (this->asset) {
        delete this->asset;
    }
}

void Resource::unload() {
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
        delete resource.second;
    }
}

ResourceManager* ResourceManager::getInstance() {
    if (!ResourceManager::instance) {
        ResourceManager::instance = new ResourceManager();
    }

    return ResourceManager::instance;
}

std::expected<void, Error> ResourceManager::loadManifest(std::string_view path) {
    if (path.ends_with(".atlas")) {
        std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
        if (!file) {
            return std::unexpected(Error::FAILED_TO_OPEN_FILE);
        }

        size_t size = file.tellg();
        file.seekg(0);

        char* data = new char[size];
        file.read(data, size);

        BufferStream stream(data, size);
        stream.seek(8); // skip header

        std::string atlas_image_path = std::string(path.substr(0, path.size() - 6));
        SpriteResource* atlas_sprite = new SpriteResource(atlas_image_path); // chop off .atlas
        this->resources.insert({atlas_image_path, atlas_sprite});
        
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

    return std::unexpected(Error::FAILED_TO_OPEN_FILE);
}

std::expected<SpriteResource*, Error> ResourceManager::getSpriteResource(std::string_view id) {
    std::string id_string(id);
    if (!this->resources.contains(id_string)) {
        return std::unexpected(Error::RESOURCE_NOT_FOUND);
    }

    SpriteResource* resource = static_cast<SpriteResource*>(this->resources.at(id_string));

    if (resource->type != ResourceType::SPRITE) {
        return std::unexpected(Error::RESOURCE_NOT_FOUND);
    }

    return resource;
}

ResourceManager* ResourceManager::instance = nullptr;

} // namespace gooforge