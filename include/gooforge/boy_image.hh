// codeshaunted - gooforge
// include/gooforge/boy_image.hh
// contains BoyImage declarations
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

#ifndef GOOFORGE_BOY_IMAGE_HH
#define GOOFORGE_BOY_IMAGE_HH

#include <expected>
#include <string_view>

#include "SFML/Graphics.hpp"

#include "error.hh"

namespace gooforge {

class BoyImage {
    public:
        static std::expected<sf::Image, Error> loadFromFile(std::string_view path);
};

} // namespace gooforge

#endif // GOOFORGE_BOY_IMAGE_HH