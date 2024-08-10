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

#include <cmath>

namespace gooforge {

float Vector2f::distance(const Vector2f& other) const {
    float dx = other.x - this->x;
    float dy = other.y - this->y;

    return std::sqrt((dx * dx) + (dy * dy));
}

Vector2f Vector2f::operator+(const Vector2f& other) const {
    return { this->x + other.x, this->y + other.y };
}

Vector2f Vector2f::operator*(float scalar) const {
    return { this->x * scalar, this->y * scalar };
}

} // namespace gooforge