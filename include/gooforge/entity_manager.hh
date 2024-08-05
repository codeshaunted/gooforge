// codeshaunted - gooforge
// include/gooforge/entity_manager.hh
// contains EntityManager declarations
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

#ifndef GOOFORGE_ENTITY_MANAGER_HH
#define GOOFORGE_ENTITY_MANAGER_HH

#include <vector>

#include "SFML/Graphics.hpp"

#include "entity.hh"

namespace gooforge {

class EntityManager {
	public:
		static EntityManager* getInstance();
		void add(Entity* entity);
		void update();
		void draw(sf::RenderWindow* window);
	private:
		static EntityManager* instance;
		std::vector<Entity*> entities;
};

} // namespace gooforge

#endif // GOOFORGE_ENTITY_MANAGER_HH