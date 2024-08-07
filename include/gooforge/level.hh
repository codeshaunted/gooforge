// codeshaunted - gooforge
// include/gooforge/level.hh
// contains Level declarations
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

#ifndef GOOFORGE_LEVEL_HH
#define GOOFORGE_LEVEL_HH

#include <expected>

#include "simdjson.h"

#include "error.hh"
#include "goo_ball.hh"
#include "goo_strand.hh"
#include "vector.hh"

namespace gooforge {

struct LevelInfo {
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
	//std::vector<TerrainGroupInfo> terrainGroups; // TODO: implement
	//std::vector<ItemInstanceInfo> items; // TODO: implement
	//std::vector<PinInfo> pins; // TODO: implement
	//std::vector<InitialKeyframeInfo> initialCameraKeyframes; // TODO: implement
	//std::vector<TerrainBallInfo> terrainBalls; // TODO: implement
	// std::vector<std::string> levelEnvironmentEffects; // TODO: implement
	bool enableTimeBugs;
	int timebugMoves;

	static std::expected<LevelInfo, Error> deserialize(simdjson::ondemand::value json);
	static std::expected<LevelInfo, Error> deserializeFile(std::string_view file_path);
};

class Level {
	public:
		Level(LevelInfo info);
		~Level();
		void addEntity(Entity* entity);
		void update();
		void draw(sf::RenderWindow* window);
		static sf::Vector2f worldToScreen(Vector2f world);
		static Vector2f screenToWorld(sf::Vector2f screen);
	private:
		void sortEntities();
		LevelInfo info;
		std::vector<Entity*> entities;
		bool entities_dirty = false;

	friend class Editor;
};

} // namespace gooforge

#endif // GOOFORGE_LEVEL_HH