// codeshaunted - gooforge
// source/gooforge/vector.cc
// contains Vector definitions
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

#include "vector.hh"

namespace gooforge {

std::expected<Vector2f, std::shared_ptr<JSONDeserializeError>> Vector2f::deserialize(simdjson::ondemand::value json) {
    Vector2f vector;

    auto x = json.find_field("x").get_double();
    if (simdjson::error_code error = x.error()) {
        return std::unexpected(std::make_shared<JSONDeserializeError>("x", error));
    }
    vector.x = x.value();

    auto y = json.find_field("y").get_double();
    if (simdjson::error_code error = y.error()) {
        return std::unexpected(std::make_shared<JSONDeserializeError>("y", error));
    }
    vector.y = y.value();

    return vector;
}

} // namespace gooforge