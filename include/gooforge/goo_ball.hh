// codeshaunted - gooforge
// include/gooforge/goo_ball.hh
// contains GooBall declarations
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

#ifndef GOOFORGE_GOO_BALL_HH
#define GOOFORGE_GOO_BALL_HH

#include <unordered_map>

#include "SFML/Graphics.hpp"

#include "entity.hh"
#include "goo_ball_template.hh"

namespace gooforge {

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

struct BallTemplateContainsInfo {
	struct {
		int ballType;
	} ballType;
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
	int color;
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

struct BallTemplateInfo {
	std::string name;
	float width;
	float height;
	struct {
		int ballShape;
	} shape;
	float sizeVariance;
	float mass;
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
	struct {
		std::string materialName;
	} material;
	std::vector<BallTemplateContainsInfo> contains;
	struct {
		std::string soundId;
	} popSoundId;
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
	struct {
		int ballType;
	} spawnType;
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
	struct {
		int strandType;
	} strandType;
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
	float strandDensity;
	float strandThickness;
	float strandGrowMultiplier;
	bool isSingleStrandAllowed;
	bool isSingleStrandAllowedEvenForOneStrand;
	bool allowBallToStrandConversion;
	bool swallowBall;
	bool useStrandConnect;
	bool isStrandWalkable;
	bool canShrinkStrand;
	BallTemplateImageIdInfo strandImageId;
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
	struct {
		int liquidType;
	} stainLiquidType;
	BallTemplateAttenuationFuncInfo deselectAttenuationFunc;
	BallTemplateAttenuationFuncInfo selectAttenuationFunc;
	BallTemplateAttenuationFuncInfo dropAttenuationFunc;
	BallTemplateAttenuationFuncInfo dragAttenuationFunc;
	BallTemplateAttenuationFuncInfo spawnAttenuationFunc;
	std::vector<BallTemplateBallPartInfo> ballParts;
	struct {
		std::string partName;
	} bodyPart;
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
	int launcherBallTypeToGenerate;
	float thrustForce;
	float maxVelocity;
	float stiffness;
	bool filled;
	float detonationRadius;
	float detonationForce;
};


class GooBall : public Entity {
	public:
		GooBall(GooBallInfo* info);
		~GooBall();
		void update() override;
		void draw(sf::RenderWindow* window) override;
		static void loadGooBallTemplates(std::string_view path);
		GooBallInfo* getInfo();
		GooBallTemplate* getTemplate();
	private:
		static std::unordered_map<std::string, GooBallType> ball_name_to_type;
		static std::unordered_map<GooBallType, GooBallTemplate> ball_templates;
		static std::unordered_map<unsigned int, GooBall*> balls;
		GooBallInfo* info;
		GooBallTemplate* ball_template = nullptr;

	friend class GooStrand;
};

} // namespace gooforge

#endif // GOOFORGE_GOO_BALL_HH