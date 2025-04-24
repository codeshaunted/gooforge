// codeshaunted - gooforge
// include/gooforge/vector.hh
// contains Vector declarations
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

#ifndef GOOFORGE_VECTOR_HH
#define GOOFORGE_VECTOR_HH

#include <expected>

#include "error.hh"

namespace gooforge {

struct Vector2f {
        float x;
        float y;

        float distance(const Vector2f& other) const;
        Vector2f abs() const;
        Vector2f operator+(const Vector2f& other) const;
        Vector2f operator-(const Vector2f& other) const;
        Vector2f operator*(float scalar) const;
        Vector2f operator*=(float scalar);
};

} // namespace gooforge

#endif // GOOFORGE_VECTOR_HH