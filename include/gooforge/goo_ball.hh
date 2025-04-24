// codeshaunted - gooforge
// include/gooforge/goo_ball.hh
// contains GooBall declarations
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

#ifndef GOOFORGE_GOO_BALL_HH
#define GOOFORGE_GOO_BALL_HH

#include <set>
#include <unordered_map>

#include "SFML/Graphics.hpp"

#include "entity.hh"
#include "terrain.hh"

namespace gooforge {

class GooStrand;

enum class GooBallType {
	INVALID = 0,
	COMMON,
	COMMON_ALBINO,
	IVY,
	BALLOON,
	GOOLF_SINGLE,
	ANCHOR,
	LAUNCHER_L2B,
	GOO_PRODUCT,
	THRUSTER,
	TERRAIN,
	BALLOON_EYE,
	CONDUIT,
	LAUNCHER_L2L,
	GOO_PRODUCT_WHITE,
	GROW,
	BOMB_STICKY,
	ROPE,
	BOUNCY,
	FISH,
	TIME_BUG,
	MATCH_STICK = 23,
	COMMON_WORLD,
	FIREWORKS,
	LIGHTBALL,
	TWU_BIT,
	TWU_BIT_BIT,
	ADAPTER,
	WINCH,
	SHRINK = 32,
	JELLY,
	GOOLF,
	THIS_WAY_UP,
	LIQUID_LEVEL_EXIT,
	EYE,
	UTIL_ATTACH_WALKABLE
};

struct BallTemplateBallTypeInfo {
	GooBallType ballType;
};

struct BallTemplateContainsInfo {
	BallTemplateBallTypeInfo ballType;
	int count;
};

struct BallTemplateParticleEffectInfo {
	std::string info;
};

struct BallTemplateImageIdInfo {
	std::string imageId;
};

struct BallTemplateAttenuationFuncInfo {
	std::vector<float> frames;
	float totalLength;
};

struct BallTemplateBallPartImageInfo {
	BallTemplateImageIdInfo imageId;
	BallTemplateImageIdInfo imageMaskId;
};

enum class GooBallState {
	WALKING = 0,
	CLIMBING,
	FALLING,
	DRAGGING,
	ATTACHED,
	DETACHING,
	DETACHED,
	STANDING,
	STUCK,
	STUCK_ATTACHED,
	STUCK_DETACHING,
	PIPE,
	TANK,
	DYING,
	PIPE_IN,
	SWIMMMING,
	DESPAWN,
	NOT_VALID_SUCKING,
	FLUSHING,
	COUNT
};

struct GooBallStateEnumInfo {
	GooBallState ballState;
};

struct BallTemplateColorInfo {
	unsigned int color;
};

struct BallTemplateBallPartInfo {
	std::string name;
	std::vector<BallTemplateBallPartImageInfo> images;
	std::vector<BallTemplateImageIdInfo> imageBackgroundIds;
	int layer;
	bool drawWhenAttached;
	bool drawWhenNotAttached;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minRangeX;
	float maxRangeX;
	float minRangeY;
	float maxRangeY;
	std::vector<GooBallStateEnumInfo> states;
	bool isActiveWhenUndiscovered;
	float scale;
	bool scaleIsRelative;
	float rotation;
	bool isEye;
	std::vector<BallTemplateImageIdInfo> pupilImageIds;
	float pupilInset;
	float pupilScale;
	bool isRotating;
	float stretchMaxSpeed;
	float stretchParallel;
	float stretchPerpendicular;
	BallTemplateColorInfo color;
	float stretchFactorFromStrandForce;
};

struct BallTemplateBallShapeInfo {
	int ballShape;
};

struct BallTemplateMaterialInfo {
	std::string materialName;
};

struct BallTemplateSoundIdInfo {
	std::string soundId;
};

struct BallTemplateLiquidTypeInfo {
	int liquidType;
};

struct BallTemplateBodyPartNameInfo {
	std::string partName;
};

struct BallTemplateInfo {
	std::string name;
	float width;
	float height;
	BallTemplateBallShapeInfo shape;
	float sizeVariance;
	/*float mass;
	float towerMass;
	float dragMass;
	float destroyForce;
	float maxDropDistance;
	int maxStrands;
	float walkSpeed;
	float climbSpeed;
	float speedVariance;
	float antiGravFactor;
	bool isAntiGravAttached;
	bool isAntiGravUnattached;
	float dampening;
	float rotationalDampening;
	bool isEditable;
	bool isDraggable;
	bool isDetachable;
	bool diesOnDetach;
	bool isInvulnerable;
	bool isInvulnerableToLava;
	bool isStickyAttached;
	bool isStickyUnattached;
	bool attachToSomeTypeOnly;
	bool isGrumpy;
	bool isStatic;
	bool isStaticWhenSleeping;
	float selectionMarkerRadius;
	bool countMoveOnUnatachedRelease;
	int attachedMaginSS;
	int unattachedMarginSS;
	int health;
	BallTemplateColorInfo blinkColor;
	float jumpMultiplierMin;
	float jumpMultiplierMax;
	bool collideAttached;
	bool collideWithAttached;
	bool collideWithParticlesAttached;
	int collisionGroup;
	bool allowAttachmentsWhenStuck;
	bool allowAttachmentsWhenFalling;
	float motorMaxForce;
	float motorMaxForceUndiscovered;
	bool isSuckable;
	bool opensExitUnattached;
	bool affectsAutoboundsAttached;
	bool affectsAutoboundsUnattached;
	float burnTime;
	float detonateForce;
	float detonateRadius;
	bool detonateKillBalls;
	bool detonateKillItems;
	bool detonateKillTerrain;
	BallTemplateParticleEffectInfo detonateParticleEffect;
	bool autoAttach;
	bool isClimber;
	float attachedParticleBarrierFactor;
	BallTemplateMaterialInfo material;
	std::vector<BallTemplateContainsInfo> contains;
	BallTemplateSoundIdInfo popSoundId;
	std::string popParticlesId;
	bool despawnTriggersFullDeath;
	BallTemplateParticleEffectInfo deathParticleEffect;
	BallTemplateParticleEffectInfo fireworksParticleEffect;
	std::string trailEffectId;
	BallTemplateParticleEffectInfo trailParticleEffect;
	bool trailEffectEnabled;
	float popDuration;
	float popDelayMin;
	float popDelayMax;
	bool hideEyes;
	bool botoxEyesWhenAttached;
	bool isBehindStrands;
	float wakeOtherBallsAtDistance;
	BallTemplateBallTypeInfo spawnType;
	float decay;
	float flingForceFactor;
	float flingStrandMaxLength;
	bool autoDisable;
	float maxDragForce;
	float dragDampeningFactor;
	bool alwaysLookAtMouse;
	bool hingeDrag;
	float maxAttachSpeed;
	bool jumpOnWakeup;
	float thrust;
	bool useDistantSounds;
	bool canBeRotatedByHand;
	int stencil;
	bool stencilWhenAttached;
	bool canSuckLiquidExternally;
	bool canSuckLiquidFromPipe;
	int liquidParticlesPerSecond;
	int initialBallLiquidAmount;
	float maxBallLiquidAmount;
	float maxStrandLiquidAmount;
	float liquidSinkOffset;
	int strandSuckLiquidParticlesPerSecond;
	float hitVelocityAccumulationLimit;
	BallTemplateBallTypeInfo strandType;
	float springConstMin;
	float springConstMax;
	float strandDampening;
	float maxReplacementStrandLength;
	float maxNormalStrandLength;
	float minStrandLength;
	float strandShrinkLength;
	float maxStretchForce;
	float maxCompressForce;
	float maxStrandAngle;
	float maxStrandSeparation;
	float strandDensity;*/
	float strandThickness;
	/*
	float strandGrowMultiplier;
	bool isSingleStrandAllowed;
	bool isSingleStrandAllowedEvenForOneStrand;
	bool allowBallToStrandConversion;
	bool swallowBall;
	bool useStrandConnect;
	bool isStrandWalkable;
	bool canShrinkStrand;*/
	BallTemplateImageIdInfo strandImageId;
	/*
	BallTemplateImageIdInfo strandInactiveImageId;
	BallTemplateImageIdInfo strandInactiveOverlayImageId;
	float strandIgniteDelay;
	float strandBurnSpeed;
	std::string strandFireParticlesId;
	BallTemplateImageIdInfo strandBurntImageId;
	BallTemplateImageIdInfo strandBackgroundImageId;
	BallTemplateImageIdInfo detachStrandImageId;
	float detachStrandMaxLength;
	BallTemplateImageIdInfo dragMarkerImageId;
	BallTemplateImageIdInfo detachMarkerImageId;
	float markerRotSpeed;
	BallTemplateLiquidTypeInfo stainLiquidType;
	BallTemplateAttenuationFuncInfo deselectAttenuationFunc;
	BallTemplateAttenuationFuncInfo selectAttenuationFunc;
	BallTemplateAttenuationFuncInfo dropAttenuationFunc;
	BallTemplateAttenuationFuncInfo dragAttenuationFunc;
	BallTemplateAttenuationFuncInfo spawnAttenuationFunc;*/
	std::vector<BallTemplateBallPartInfo> ballParts;
	BallTemplateBodyPartNameInfo bodyPart;
	//std::vector<BallTemplateStateAnimationInfo> stateAnimations; // TODO: implement?
	// TODO: implement the rest of this?
};

struct GooBallInfo {
	GooBallType typeEnum;
	int uid;
	Vector2f pos;
	float angle;
	bool discovered;
	bool floatingWhileAsleep;
	bool interactive;
	bool wakeWithLiquid;
	bool exitPipeAlert;
	bool affectsAutoBounds;
	float launcherLifespanMin;
	float launcherLifespanMax;
	float launcherForceFactor;
	bool launcherCanUseBalls;
	float launcherKnockbackFactor;
	int launcherMaxActive;
	GooBallType launcherBallTypeToGenerate;
	float thrustForce;
	float maxVelocity;
	float stiffness;
	bool filled;
	float detonationRadius;
	float detonationForce;
};


class GooBall : public Entity {
	public:
		GooBall() : Entity(EntityType::GOO_BALL) {}
		~GooBall() override;
		std::expected<void, Error> setup(GooBallInfo info, std::weak_ptr<TerrainGroup> terrain_group);
		std::expected<void, Error> refresh() override;
		void update() override;
		void draw(sf::RenderWindow* window) override;
		sf::Sprite getThumbnail() override;
		std::string getDisplayName() override;
		Vector2f getPosition() override;
		float getRotation() override;
		float getDepth() const override;
		void setPosition(Vector2f position) override;
		GooBallInfo& getInfo();
		BallTemplateInfo* getTemplate();
		std::weak_ptr<TerrainGroup> getTerrainGroup();
		static std::unordered_map<std::string, GooBallType> ball_name_to_type;
		static std::unordered_map<GooBallType, std::string> ball_type_to_name;
		std::set<std::weak_ptr<GooStrand>, std::owner_less<std::weak_ptr<GooStrand>>> getStrands();
		void notifyAddStrand(std::shared_ptr<GooStrand> strand) override;
		void notifyRemoveStrand(std::shared_ptr<GooStrand> strand) override;
	private:
		std::weak_ptr<TerrainGroup> terrain_group;
		GooBallInfo info;
		BallTemplateInfo* ball_template = nullptr;
		BallTemplateBallPartInfo* body_part = nullptr;
		std::set<std::weak_ptr<GooStrand>, std::owner_less<std::weak_ptr<GooStrand>>> strands;
		sf::Sprite display_sprite;

	friend class GooStrand;
	friend class TerrainGroup;
	friend class Level;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_BALL_HH