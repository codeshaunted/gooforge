// codeshaunted - gooforge
// include/gooforge/item.hh
// contains ItemInstance and related declarations
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

#ifndef GOOFORGE_ITEM_HH
#define GOOFORGE_ITEM_HH

#include "entity.hh"

namespace gooforge {

struct ItemInstanceUserVariableInfo {
	float value;
};

struct ItemInstanceInfo {
	std::string id;
	std::string type;
	std::string localizedStringId;
	unsigned int uid;
	Vector2f pos;
	Vector2f scale;
	float rotation;
	float depth;
	bool flipHorizontal;
	bool flipVertical;
	float rotSpeed;
	int seed;
	int liquidType; 
	unsigned int adapterBallId;
	bool invisible;
	int forcedRandomizationIndex;
	std::string particleEffectName;
	int uid1;
	int uid2;
	std::vector<ItemInstanceUserVariableInfo> userVariables;
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

struct ItemUserVariableInfo {
	std::string name;
	float defaultValue;
	float minValue;
	float maxValue;
	int orderIndex;
	bool enabled;
	int type;
	std::string stringValue;
};

struct ItemLimitsInfo {
	float minScale;
	float maxScale;
};

struct ItemInfo {
	std::string name;
	std::string uuid;
	int type;
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

class ItemInstance : public Entity {
public:
	ItemInstance() : Entity(EntityType::ITEM_INSTANCE) {}
	~ItemInstance() override;
	void update() override;
	void draw(sf::RenderWindow* window) override;
private:
	ItemInstanceInfo* info;
	ItemInfoFile* info_file;
};

} // namespace gooforge

#endif // GOOFORGE_ITEM_HH