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

Vector2f Vector2f::scale(float scalar) {
    Vector2f scaled;

    scaled.x = this->x * scalar;
    scaled.y = this->y * scalar;

    return scaled;
}

float Vector2f::distance(const Vector2f& other) const {
    float dx = other.x - this->x;
    float dy = other.y - this->y;

    return std::sqrt((dx * dx) + (dy * dy));
}

std::expected<Vector2f, Error> Vector2f::deserialize(simdjson::ondemand::value json) {
    Vector2f vector;

    auto x_field = json.find_field("x").get_double();
    if (simdjson::error_code error = x_field.error()) {
        return std::unexpected(JSONDeserializeError("x", error));
    }
    vector.x = x_field.value();

    auto y_field = json.find_field("y").get_double();
    if (simdjson::error_code error = y_field.error()) {
        return std::unexpected(JSONDeserializeError("y", error));
    }
    vector.y = y_field.value();

    return vector;
}

} // namespace gooforge