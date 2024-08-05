// codeshaunted - gooforge
// source/gooforge/goo_ball.cc
// contains GooBall definitions
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

#include "goo_ball.hh"

#include "resource_manager.hh"

namespace gooforge {

void GooBall::update() {

}

void GooBall::draw(sf::RenderWindow* window) {
	window->draw(*(*ResourceManager::getInstance()->getSpriteResource("IMAGE_BALL_COMMON_BODY"))->get());
}

std::unordered_map<std::string, GooBallType> GooBall::ball_name_to_type = {
	{"Invalid", GooBallType::INVALID},
	{"Common", GooBallType::COMMON},
	{"CommonAlbino", GooBallType::COMMON_ALBINO},
	{"Ivy", GooBallType::IVY},
	{"Balloon", GooBallType::BALLOON},
	{"GoolfSingle", GooBallType::GOOLF_SINGLE},
	{"Anchor", GooBallType::ANCHOR},
	{"LauncherL2B", GooBallType::LAUNCHER_L2B},
	{"GooProduct", GooBallType::GOO_PRODUCT},
	{"Thruster", GooBallType::THRUSTER},
	{"Terrain", GooBallType::TERRAIN},
	{"BalloonEye", GooBallType::BALLOON_EYE},
	{"Conduit", GooBallType::CONDUIT}
};

} // namespace gooforge