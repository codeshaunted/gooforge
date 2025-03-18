// codeshaunted - gooforge
// source/gooforge/resource_manager.cc
// contains ResourceManager definitions
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

#include "resource_manager.hh"

#include <fstream>
#include <regex>

#include "glaze/json/read.hpp"
#include "pugixml.hpp"
#include "spdlog.h"

#include "boy_image.hh"
#include "buffer_stream.hh"

namespace gooforge {

BaseResource::~BaseResource() {
    this->unload();
}

std::expected<sf::Sprite, Error> SpriteResource::get() {
    if (this->atlas_sprite) {
        auto atlas_sprite_resource = ResourceManager::getInstance()->getResource<SpriteResource>(this->path);
        if (!atlas_sprite_resource) {
            return std::unexpected(atlas_sprite_resource.error());
        }

        const sf::Texture* atlas_texture = atlas_sprite_resource.value()->get()->getTexture();

        return sf::Sprite(*atlas_texture, this->atlas_rect);
    }

    if (this->texture) {
        return sf::Sprite(*this->texture);
    } else {
        this->texture = new sf::Texture();
        auto image = BoyImage::loadFromFile(this->path);
        if (!image) {
            return std::unexpected(image.error());
        }

        this->texture->loadFromImage(*image);

        return this->get();
    }
}

void SpriteResource::unload() {
    delete this->texture;
    this->texture = nullptr;
}

std::expected<BallTemplateInfo*, Error> BallTemplateResource::get() {
    if (!this->info) {
        BallTemplateInfo* template_info = new BallTemplateInfo();
        std::string buffer;
        auto template_info_error = glz::read_file_json<glz::opts{ .error_on_unknown_keys = false }>(template_info, this->path, buffer);

        if (template_info_error) {
            delete template_info;
            return std::unexpected(JSONDeserializeError(this->path, glz::format_error(template_info_error, buffer)));
        }
        else {
            this->info = template_info;
        }
    }

    return this->info;
}

void BallTemplateResource::unload() {
    delete this->info;
    this->info = nullptr;
}

std::expected<TerrainTemplateInfoFile*, Error> TerrainTemplatesResource::get() {
    if (!this->info_file) {
        TerrainTemplateInfoFile* template_info_file = new TerrainTemplateInfoFile();
        std::string buffer;
        auto template_info_file_error = glz::read_file_json<glz::opts{ .error_on_unknown_keys = false }>(template_info_file, this->path, buffer);

        if (template_info_file_error) {
            delete template_info_file;
            return std::unexpected(JSONDeserializeError(this->path, glz::format_error(template_info_file_error, buffer)));
        }
        else {
            this->info_file = template_info_file;
        }
    }

    return this->info_file;
}

void TerrainTemplatesResource::unload() {
    delete this->info_file;
    this->info_file = nullptr;
}

std::expected<ItemInfoFile*, Error> ItemResource::get() {
    if (!this->info_file) {
        ItemInfoFile* item_info_file = new ItemInfoFile();
        std::string buffer;
        auto item_info_file_error = glz::read_file_json<glz::opts{ .error_on_unknown_keys = false }>(item_info_file, this->path, buffer);

        if (item_info_file_error) {
            delete item_info_file;
            return std::unexpected(JSONDeserializeError(this->path, glz::format_error(item_info_file_error, buffer)));
        }
        else {
            this->info_file = item_info_file;
        }
    }

    return this->info_file;
}

void ItemResource::unload() {
    delete this->info_file;
    this->info_file = nullptr;
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
        if (path.extension() == ".xml" || path.extension() == ".resrc") {
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
        else if (path.extension() == ".wog2" && path.parent_path().stem() == "items") {
            ItemResource resource(path.string());
            std::string id = "GOOFORGE_ITEM_RESOURCE_" + path.replace_extension("").filename().string();

            this->resources.insert({ id, new Resource(resource) });
        }
    }

    // load ball templates
    for (auto pair : GooBall::ball_name_to_type) {
        std::string id = "GOOFORGE_BALL_TEMPLATE_RESOURCE_" + std::to_string(static_cast<int>(pair.second));
        BallTemplateResource resource((base_path / "res/balls/" / pair.first / "ball.wog2").string());

        this->resources.insert({ id, new Resource(resource) });
    }

    // load terrain templates
    std::filesystem::path terrain_path = base_path / "res/terrain/terrain.wog2";
    TerrainTemplatesResource terrain_templates_resource(terrain_path.string());
    this->resources.insert({ "GOOFORGE_TERRAIN_TEMPLATES_RESOURCE", new Resource(terrain_templates_resource) });

    return std::expected<void, Error>{};
}

std::expected<void, Error> ResourceManager::loadResourceManifest(std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        return std::unexpected(FileOpenError(path.string()));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xml_content = buffer.str();
    // they left out quotes on one of the ids for some reason, which messes with pugixml
    // so we have to fix it with this regex replace
    // TODO: check if this fails?
    xml_content = std::regex_replace(xml_content, std::regex(R"(id=(\w+))"), R"(id="\1")");

    pugi::xml_document document;
    pugi::xml_parse_result result = document.load_string(xml_content.c_str());

    if (!result) {
        return std::unexpected(XMLDeserializeError(path.string(), result.description()));
    }

    pugi::xml_node root = document.child("ResourceManifest");

    for (auto resource = root.child("Resources"); resource; resource = resource.next_sibling("Resources")) {
        pugi::xml_node set_defaults = resource.child("SetDefaults");

        std::filesystem::path resource_path_base = this->base_path;
        std::string resource_id_prefix;
        if (set_defaults) {
            resource_path_base /= set_defaults.attribute("path").as_string();
            resource_id_prefix = set_defaults.attribute("idprefix").as_string();
        }

        for (auto image = resource.child("Image"); image; image = image.next_sibling("Image")) {
            std::string resource_id = resource_id_prefix + image.attribute("id").as_string();
            std::filesystem::path resource_path = resource_path_base / image.attribute("path").as_string();
            resource_path.replace_extension(".image");

            SpriteResource sprite_resource(resource_path.string());
            this->resources.insert({ resource_id, new Resource(sprite_resource)});
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
    SpriteResource atlas_sprite(path.string()); // chop off .atlas
    this->resources.insert({ path.string(), new Resource(atlas_sprite) });
        
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

        SpriteResource sprite_resource(path.string(), rect);
        this->resources.insert({id, new Resource(sprite_resource)});
    }

    return std::expected<void, Error>{};
}

void ResourceManager::unloadAll() {
    for (auto& [id, resource] : this->resources) {
        BaseResource* base_resource = std::visit([](auto& derived_resource) -> BaseResource* { return &derived_resource; }, *resource);

        base_resource->unload();
    }
}

ResourceManager* ResourceManager::instance = nullptr;

} // namespace gooforge