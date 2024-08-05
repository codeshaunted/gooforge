// codeshaunted - gooforge
// source/gooforge/goo_ball_template.cc
// contains GooBallTemplate definitions
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

#include "goo_ball_template.hh"

namespace gooforge {

GooBallTemplate GooBallTemplate::deserializeFromFile(std::string_view path) {
	simdjson::ondemand::parser parser;
	simdjson::padded_string json = simdjson::padded_string::load(path);
	simdjson::ondemand::document ball = parser.iterate(json);

	// TODO: JSON error handling

	GooBallTemplate ball_template;
	ball_template.name = ball.find_field("name").get_string().take_value();
	ball_template.width = ball.find_field("width").get_double().take_value();
	ball_template.height = ball.find_field("height").get_double().take_value();
	ball_template.max_strands = ball.find_field("maxStrands").get_uint64().take_value();
	ball_template.strandThickness = ball.find_field("strandThickness").get_double().take_value();
	ball_template.strand_image_id = ball.find_field("strandImageId").find_field("imageId").get_string().take_value();
	// this is somewhat sketchy as it relies upon the body being the first ball part... TODO: fix
	auto body_image = ball.find_field("ballParts").at(0).find_field("images").at(0);
	if (body_image.error() == simdjson::error_code::SUCCESS) {
		ball_template.body_image_id = body_image.find_field("imageId").find_field("imageId").get_string().take_value();
	}
	
	return ball_template;
}

} // namespace gooforge