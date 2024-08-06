// codeshaunted - gooforge
// source/gooforge/level.cc
// contains Level definitions
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

#include "level.hh"

#include "spdlog.h"

namespace gooforge {

std::expected<LevelInfo, std::shared_ptr<JSONDeserializeError>> LevelInfo::deserialize(simdjson::ondemand::value json) {
	LevelInfo info;

	auto version = json.find_field("version").get_int64();
	if (simdjson::error_code error = version.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("version", error));
	}
	info.version = version.value();

	auto type = json.find_field("type").get_int64();
	if (simdjson::error_code error = type.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("type", error));
	}
	info.type = type.value();

	auto forceFadeEOL = json.find_field("forceFadeEOL").get_bool();
	if (simdjson::error_code error = forceFadeEOL.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("forceFadeEOL", error));
	}
	info.forceFadeEOL = forceFadeEOL.value();

	auto skin = json.find_field("skin").get_int64();
	if (simdjson::error_code error = skin.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("skin", error));
	}
	info.skin = skin.value();

	auto uuid = json.find_field("uuid").get_string();
	if (simdjson::error_code error = uuid.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("uuid", error));
	}
	info.uuid = uuid.value();

	auto title = json.find_field("title").get_string();
	if (simdjson::error_code error = title.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("title", error));
	}
	info.title = title.value();

	auto startingUID = json.find_field("startingUID").get_int64();
	if (simdjson::error_code error = startingUID.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("startingUID", error));
	}
	info.startingUID = startingUID.value();

	auto island = json.find_field("island").get_int64();
	if (simdjson::error_code error = island.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("island", error));
	}
	info.island = island.value();

	auto environmentId = json.find_field("environmentId").get_int64();
	if (simdjson::error_code error = environmentId.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("environmentId", error));
	}
	info.environmentId = environmentId.value();

	auto backgroundId = json.find_field("backgroundId").get_string();
	if (simdjson::error_code error = backgroundId.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("backgroundId", error));
	}
	info.backgroundId = backgroundId.value();

	auto gravity = Vector2f::deserialize(json.find_field("gravity"));
	if (!gravity) {
		auto error = gravity.error();
		error->prependField("gravity");
		return std::unexpected(error);
	}
	info.gravity = gravity.value();

	auto boundsTopRight = Vector2f::deserialize(json.find_field("boundsTopRight"));
	if (!boundsTopRight) {
		auto error = boundsTopRight.error();
		error->prependField("boundsTopRight");
		return std::unexpected(error);
	}
	info.boundsTopRight = boundsTopRight.value();

	auto boundsBottomLeft = Vector2f::deserialize(json.find_field("boundsBottomLeft"));
	if (!boundsBottomLeft) {
		auto error = boundsBottomLeft.error();
		error->prependField("boundsBottomLeft");
		return std::unexpected(error);
	}
	info.boundsBottomLeft = boundsBottomLeft.value();

	auto initialCameraPos = Vector2f::deserialize(json.find_field("initialCameraPos"));
	if (!initialCameraPos) {
		auto error = initialCameraPos.error();
		error->prependField("initialCameraPos");
		return std::unexpected(error);
	}
	info.initialCameraPos = initialCameraPos.value();

	auto initialCameraZoom = json.find_field("initialCameraZoom").get_double();
	if (simdjson::error_code error = initialCameraZoom.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("initialCameraZoom", error));
	}
	info.initialCameraZoom = initialCameraZoom.value();

	auto cameraAutoBounds = json.find_field("cameraAutoBounds").get_bool();
	if (simdjson::error_code error = cameraAutoBounds.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("cameraAutoBounds", error));
	}
	info.cameraAutoBounds = cameraAutoBounds.value();

	auto ballsRateRequired = json.find_field("ballsRateRequired").get_double();
	if (simdjson::error_code error = ballsRateRequired.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("ballsRateRequired", error));
	}
	info.ballsRateRequired = ballsRateRequired.value();

	auto musicId = json.find_field("musicId").get_string();
	if (simdjson::error_code error = musicId.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("musicId", error));
	}
	info.musicId = musicId.value();

	auto ambienceId = json.find_field("ambienceId").get_string();
	if (simdjson::error_code error = ambienceId.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("ambienceId", error));
	}
	info.ambienceId = ambienceId.value();

	auto liquidScale = json.find_field("liquidScale").get_double();
	if (simdjson::error_code error = liquidScale.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("liquidScale", error));
	}
	info.liquidScale = liquidScale.value();

	auto pretickSeconds = json.find_field("pretickSeconds").get_double();
	if (simdjson::error_code error = pretickSeconds.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("pretickSeconds", error));
	}
	info.pretickSeconds = pretickSeconds.value();

	auto conduitSuckVolume = json.find_field("conduitSuckVolume").get_double();
	if (simdjson::error_code error = conduitSuckVolume.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("conduitSuckVolume", error));
	}
	info.conduitSuckVolume = conduitSuckVolume.value();

	auto fireSfxVolume = json.find_field("fireSfxVolume").get_double();
	if (simdjson::error_code error = fireSfxVolume.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("fireSfxVolume", error));
	}
	info.fireSfxVolume = fireSfxVolume.value();

	// TODO: implement missing fields
	
	auto balls = json.find_field("balls").get_array();
	if (simdjson::error_code error = balls.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("balls", error));
	}
	size_t ball_i = 0;
	for (auto ball : balls.value()) {
		if (simdjson::error_code error = ball.error()) {
			return std::unexpected(std::make_shared<JSONDeserializeError>("balls", ball_i, error));
		}
		auto ball_info = GooBallInfo::deserialize(ball.value());
		
		if (!ball_info) {
			auto error = ball_info.error();
			error->prependFieldAndArrayIndex("balls", ball_i);
			return std::unexpected(error);
		}

		info.balls.push_back(ball_info.value());

		++ball_i;
	}

	// GOOSTRAND

	auto enableTimeBugs = json.find_field("enableTimeBugs").get_bool();
	if (simdjson::error_code error = enableTimeBugs.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("enableTimeBugs", error));
	}
	info.enableTimeBugs = enableTimeBugs.value();

	auto timebugMoves = json.find_field("timebugMoves").get_int64();
	if (simdjson::error_code error = timebugMoves.error()) {
		return std::unexpected(std::make_shared<JSONDeserializeError>("timebugMoves", error));
	}
	info.timebugMoves = timebugMoves.value();
	
	return info;
}

Level::Level(LevelInfo info) {

}

Level::~Level() {
	for (auto entity : this->entities) {
		delete entity;
	}
}

void Level::update() {
	for (auto entity : this->entities) {
		entity->update();
	}
}

void Level::draw(sf::RenderWindow* window) {
	if (this->entities_dirty) {
		std::sort(this->entities.begin(), this->entities.end(), [](const Entity* a, const Entity* b) {
			return a->layer < b->layer;
			});

		this->entities_dirty = false;
	}

	for (auto entity : this->entities) {
		entity->draw(window);
	}
}

} // namespace gooforge