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
        Vector2f textureOffset = Vector2f(0.0f, 0.0f);
        std::string typeUuid = "177636a8-b061-465a-8cbb-a2241fa429d8";
        int typeIndex = 6;
        int sortOffset = 0;
        float depth = 0.0f;
        bool foreground = true;
        bool collision = true;
        bool destructable = false;
        bool buildable = false;
        bool occluder = true;
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
        void setDepth(float depth) override;
        TerrainGroupInfo& getInfo();
        std::string getTerrainTemplateUUID();
        void setTerrainTemplateUUID(std::string uuid);
        int getSortOffset() const;
        void setSortOffset(int offset);
        void notifyAddStrand(GooStrand* strand) override;
        void notifyRemoveStrand(GooStrand* strand) override;
        void notifyUpdateStrand(GooStrand* strand) override;

    private:
        TerrainGroupInfo info;
        TerrainTemplateInfo* template_info;
        std::unordered_set<GooStrand*> terrain_strands;
        sf::Sprite display_sprite;
};

} // namespace gooforge

#endif // GOOFORGE_TERRAIN_HH