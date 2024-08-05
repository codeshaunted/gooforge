// codeshaunted - gooforge
// include/gooforge/entity.hh
// contains Entity declarations
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

#ifndef GOOFORGE_ENTITY_HH
#define GOOFORGE_ENTITY_HH

#include "SFML/Graphics.hpp"

#include "positionable.hh"

namespace gooforge {

class Entity : public Positionable {
	public:
		virtual void update() {}
		virtual void draw(sf::RenderWindow* window) {}
};

} // namespace gooforge

#endif // GOOFORGE_ENTITY_HH