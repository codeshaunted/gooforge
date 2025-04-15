// codeshaunted - gooforge
// include/gooforge/level.hh
// contains Level declarations
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

#ifndef GOOFORGE_LEVEL_HH
#define GOOFORGE_LEVEL_HH

#include <expected>
#include <set>

#include "error.hh"
#include "goo_ball.hh"
#include "goo_strand.hh"
#include "item.hh"
#include "terrain.hh"
#include "vector.hh"

namespace gooforge {

struct PinInfo {
	unsigned int uid;
	Vector2f pos;
	float damping;
	bool limitVelocity;
	float maxVelocity;
};

struct InitialCameraKeyframeInfo {
	Vector2f position;
	float zoom;
	float timeScaleFactor;
	float timeScaleFactor2;
	float pauseTimeFactor;
	float easeInSpeedFactor;
	float easeOutSpeedFactor;
};

struct TerrainBallInfo {
	int group;
};

struct LevelInfo {
	std::string _gooforge = "This file was generated by gooforge v0.1";
	int version;
	int type;
	bool forceFadeEOL;
	int skin;
	std::string uuid;
	std::string title;
	int startingUID;
	int island;
	int environmentId;
	std::string backgroundId;
	Vector2f gravity;
	Vector2f boundsTopRight;
	Vector2f boundsBottomLeft;
	Vector2f initialCameraPos;
	float initialCameraZoom;
	bool cameraAutoBounds;
	float ballsRateRequired;
	std::string musicId;
	std::string ambienceId;
	float liquidScale;
	float pretickSeconds;
	float conduitSuckVolume;
	float fireSfxVolume;
	std::vector<GooBallInfo> balls;
	std::vector<GooStrandInfo> strands;
	std::vector<TerrainGroupInfo> terrainGroups;
	std::vector<ItemInstanceInfo> items;
	std::vector<PinInfo> pins;
	std::vector<InitialCameraKeyframeInfo> initialCameraKeyframes;
	std::vector<TerrainBallInfo> terrainBalls;
	std::vector<std::string> levelEnvironmentEffects;
	bool enableTimeBugs;
	int timebugMoves;
};

struct EntityDepthComparator {
	// sort by depth for drawing
	bool operator()(const Entity* x, const Entity* y) const {
		if (x->getDepth() != y->getDepth()) {
			return x->getDepth() < y->getDepth();
		}

		// if they're at the same depth we need an alternative so they don't get deleted as duplicates
		return x < y; // cry about it
	}
};

class Level {
	public:
		~Level();
		std::expected<void, Error> setup(LevelInfo info);
		void update();
		void draw(sf::RenderWindow* window);
		static sf::Vector2f worldToScreen(Vector2f world);
		static Vector2f screenToWorld(sf::Vector2f screen);
		static float radiansToDegrees(float radians);
		static float degreesToRadians(float degrees);
		void deleteEntity(Entity* entity);
	private:
		LevelInfo info;
		std::set<Entity*, EntityDepthComparator> entities;
		bool entities_dirty = false;

	friend class Editor;
};

} // namespace gooforge

#endif // GOOFORGE_LEVEL_HH