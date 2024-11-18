// codeshaunted - gooforge
// source/gooforge/vector.cc
// contains Vector definitions
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