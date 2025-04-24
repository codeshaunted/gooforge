// codeshaunted - gooforge
// include/gooforge/terrain.hh
// contains Terrain and related declarations
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

#ifndef GOOFORGE_TERRAIN_HH
#define GOOFORGE_TERRAIN_HH

#include <expected>
#include <set>

namespace gooforge {

class GooBall;
class GooStrand;

struct TerrainGroupInfo {
        Vector2f textureOffset;
        std::string typeUuid;
        int typeIndex;
        int sortOffset;
        float depth;
        bool foreground;
        bool collision;
        bool destructable;
        bool buildable;
        bool occluder;
};

struct TerrainTemplateImageIdInfo {
        std::string imageId;
};

struct TerrainTemplateBaseSettingsInfo {
        TerrainTemplateImageIdInfo image;
};

struct TerrainTemplateInfo {
        std::string uuid;
        std::string name;
        TerrainTemplateBaseSettingsInfo baseSettings;
};

struct TerrainTemplateInfoFile {
        std::vector<TerrainTemplateInfo> terrainTypes;
};

class TerrainGroup : public Entity {
    public:
        TerrainGroup() : Entity(EntityType::TERRAIN_GROUP) {}
        std::expected<void, Error> setup(TerrainGroupInfo info);
        std::expected<void, Error> refresh();
        void update() override;
        void draw(sf::RenderWindow* window) override;
        std::string getDisplayName() override;
        sf::Sprite getThumbnail() override;
        float getDepth() const override;
        TerrainGroupInfo& getInfo();
        void notifyAddStrand(std::shared_ptr<GooStrand> strand) override;
        void notifyRemoveStrand(std::shared_ptr<GooStrand> strand) override;

    private:
        TerrainGroupInfo info;
        TerrainTemplateInfo* template_info;
        std::set<std::weak_ptr<GooStrand>,
                 std::owner_less<std::weak_ptr<GooStrand>>>
            terrain_strands;
        sf::Sprite display_sprite;
};

} // namespace gooforge

#endif // GOOFORGE_TERRAIN_HH