// codeshaunted - gooforge
// include/gooforge/goo_ball_template.hh
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

#ifndef GOOFORGE_GOO_BALL_TEMPLATE_HH
#define GOOFORGE_GOO_BALL_TEMPLATE_HH

#include "simdjson.h"

namespace gooforge {

struct GooBallTemplate {
	std::string name;
	double width;
	double height;
	uint64_t max_strands;
	std::string body_image_id;
	std::string strand_image_id;

	static GooBallTemplate deserializeFromFile(std::string_view path);
};

} // namespace gooforge

#endif // GOOFORGE_GOO_BALL_TEMPLATE_HH