// codeshaunted - gooforge
// include/gooforge/item.hh
// contains ItemInstance and related declarations
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

#ifndef GOOFORGE_ITEM_HH
#define GOOFORGE_ITEM_HH

#include <unordered_map>

#include "entity.hh"

namespace gooforge {

struct ItemInstanceUserVariableInfo {
        float value;
};

struct ItemInstanceInfo {
        std::string id = "";
        std::string type = "61af9274-1ca4-4599-afa4-aa21e3b02580";
        std::string localizedStringId = "";
        unsigned int uid = 0;
        Vector2f pos = Vector2f(0.0f, 0.0f);
        Vector2f scale = Vector2f(1.0f, 1.0f);
        float rotation = 0.0f;
        float depth = 0.0f;
        bool flipHorizontal = false;
        bool flipVertical = false;
        float rotSpeed = 1.0f;
        int seed = 0;
        int liquidType = 0;
        unsigned int adapterBallId = 0;
        bool invisible = false;
        int forcedRandomizationIndex = -1;
        std::string particleEffectName = "";
        int uid1 = 0;
        int uid2 = 0;
        std::vector<ItemInstanceUserVariableInfo> userVariables = {};
};

enum ItemType {
    INVALID = 0,
    GENERIC,
    PIPE_IN_LIQUID,
    CONNECTION,
    PIPE_IN,
    TILABLE,
    GOD_RAY_AREA,
    LINEAR_FORCE_FIELD,
    RADIAL_FORCE_FIELD,
    POOL,
    RANGE_END_CRITERIA,
    TORCH,
    TERRAIN_ATTRIB,
    WATER,
    SWING,
    FIREBALL,
    GEAR,
    DEBRIS,
    ROPE,
    FIREWORKS_BOAT,
    JELLY_CUBES_PIPE,
    PISTON,
    DECORATION,
    LEVEL_EXIT,
    TIMED_FADE,
    SIGN_PAINTER_SIGN,
    TERRAIN_HIDE,
    TIMED_ANIMATION,
    SELECT_ANIMATION,
    BUTTON,
    DRAIN,
    WINCH,
    SQUIDDY,
    PIN_TO_JELLY,
    BALL_DEBRIS,
    RAIL_CONSTRAINT,
    SATELLITE_DISH,
    CAMERA_CONTROL = 38,
    CAMERA_EOL,
    ISLAND2,
    PARTICLE_EFFECT_IN_LEVEL,
    EYE,
    AMBIENT_SOUND_AREA,
    AUTO_BOUNDS_AREA,
    ROCKET,
    LEVEL_TEXT,
    TENTACLE,
    OVERLAY,
    OCD_FLAG,
    ANGLER_HOOK
};

struct Geom {
        int type;
        float width;
        float height;
        float radius;
        float density;
        Vector2f offset;
        float rotation;
        Vector2f normal;
        bool isSensor;
        float friction;
        float bounciness;
        std::vector<Vector2f> points;
};

struct ItemColorFXInfo {
        bool enabled;
        float brightness;
        float contrast;
        float exposure;
        float saturation;
};

struct PhysicsBodyInfo {
        std::string id;
        int type;
        Vector2f pos;
        float rotation;
        bool collisionEnabled;
        std::string materialName;
        std::string tags;
        std::vector<Geom> geoms;
        int categoryBits;
};

struct ItemParticleEffectInfo {
        std::string name;
        Vector2f position;
        float rotation;
        float scale;
        float depth;
        int sortOffset;
        bool relativeDepthAndSort;
        bool enabled;
};

struct ItemPointInfo {
        Vector2f position;
        int type;
        float rotation;
        float radius;
        int userValue;
};

struct ItemObjectInfo {
        std::string name;
        int randomizationGroup;
        Vector2f position;
        float rotation;
        Vector2f scale;
        unsigned int color;
        float depthOffset;
        int sortOffset;
        int imageBlendingType;
        Vector2f pivot;
        float rotationSpeed;
        bool invisible;
        bool clickable;
        unsigned int stableFluidsDensityFactor;
        float dynamicLightingFactor;
        bool flipHorizontal;
        bool flipVertical;
        bool ignoreScale;
        float shaderFactor;
        bool enableWind;
        Vector2f windFactor;
        unsigned int stencilMode;
        unsigned int stencilMask;
        unsigned int alphaTestValue;
        PhysicsBodyInfo body;
        ItemColorFXInfo colorFx;
        std::vector<ItemParticleEffectInfo> particleEffects;
        std::vector<ItemPointInfo> points;
};

struct ItemAlternateInfo {
        std::string stringValue;
        int integerValue;
};

// custom, not found in ghidra
enum class ItemUserVariableType { FLOAT = 0, INT, BOOL, LIQUID_TYPE, BALL_TYPE };

struct ItemUserVariableInfo {
        std::string name;
        float defaultValue;
        float minValue;
        float maxValue;
        int orderIndex;
        bool enabled;
        ItemUserVariableType type;
        std::string stringValue;
};

struct ItemLimitsInfo {
        float minScale;
        float maxScale;
};

struct ItemInfo {
        std::string name;
        std::string uuid;
        ItemType type;
        std::string category;
        Layer layer;
        float minDepth;
        float maxDepth;
        int sortOffset;
        float radiusOerride;
        bool canRotate;
        bool uniformScale;
        bool canBeCopied;
        bool canTriggerEndOfLevel;
        bool variations;
        bool useLighting;
        bool castShadow;
        bool checkVisibility;
        unsigned int color;
        std::string animationName;
        std::string animationAlias;
        float animationSpeed;
        float animationInNormalizedTime;
        Vector2f animationPlayNormalizedTimeRange;
        Vector2f animationLocalPosition;
        Vector2f animationLocalScale;
        float animationRotation;
        float userValue1;
        float userValue2;
        int snapObjectIndex;
        float destroyForce;
        std::vector<ItemObjectInfo> objects;
        std::vector<ItemAlternateInfo> alternates;
        std::vector<ItemUserVariableInfo> userVariables;
        std::vector<Vector2f> snapPoints;
        ItemLimitsInfo limits;
        // TODO: implement the rest of the fields?
};

struct ItemInfoFile {
        std::vector<ItemInfo> items;
};

enum class LiquidType {
    GOO = 0,
    WATER,
    COOLED_LAVA,
    LAVA,
    JELLY_BLOCK_HARD,
    JELLY_BLOCK_SOFT
};

static std::unordered_map<LiquidType, std::string> liquid_type_to_name =
    {
        {LiquidType::GOO, "Goo"},
        {LiquidType::WATER, "Water"},
        {LiquidType::COOLED_LAVA, "Cooled Lava"},
        {LiquidType::LAVA, "Lava"},
        {LiquidType::JELLY_BLOCK_HARD, "Jelly Block Hard"},
        {LiquidType::JELLY_BLOCK_SOFT, "Jelly Block Soft"},
};

class ItemInstance : public Entity {
    public:
        ItemInstance() : Entity(EntityType::ITEM_INSTANCE) {}
        ~ItemInstance() override;
        static std::unordered_map<ItemType, std::string> item_type_to_name;
        std::expected<void, Error> setup(ItemInstanceInfo info);
        std::expected<void, Error> refresh() override;
        void update() override;
        void draw(sf::RenderWindow* window) override;
        sf::Sprite getThumbnail() override;
        std::string getDisplayName() override;
        Vector2f getPosition() override;
        float getRotation() override;
        void setRotation(float rotation) override;
        float getDepth() const override;
        void setDepth(float depth) override;
        void setPosition(Vector2f position) override;
        Vector2f getScale();
        void setScale(Vector2f scale);
        std::string getItemTemplateUUID();
        void setItemTemplateUUID(std::string uuid);
        int getForcedRandomizationIndex();
        void setForcedRandomizationIndex(int index);
        ItemInstanceInfo& getInfo();
        ItemType getItemType();
        std::vector<ItemUserVariableInfo> getUserVariableInfo();
        template <typename T>
        T getUserVariableValue(size_t index);
        template <typename T>
        void setUserVariableValue(size_t index, T value);
        std::vector<ItemInstanceUserVariableInfo> getUserVariableValues();
        void setUserVariableValues(
            std::vector<ItemInstanceUserVariableInfo> values);

    private:
        ItemInstanceInfo info;
        ItemInfoFile* info_file;
        ItemObjectInfo* object_info;
        sf::Sprite display_sprite;
};

// todo: error checking?
template <typename T>
T ItemInstance::getUserVariableValue(size_t index) {
    return T(this->info.userVariables[index].value);
}

template <typename T>
void ItemInstance::setUserVariableValue(size_t index, T value) {
    this->info.userVariables[index].value = float(value);
}

} // namespace gooforge

#endif // GOOFORGE_ITEM_HH