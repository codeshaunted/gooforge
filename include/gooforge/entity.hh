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

#include "vector.hh"

namespace gooforge {

enum class Layer {
	BACKGROUND = 0,
	TERRAIN,
	GAMEPLAY,
	FOREGROUND,
	MISC,
	DECORATIONS
};

enum class EntityClickBoundShapeType {
	CIRCLE
};

struct EntityClickBoundShape {
	EntityClickBoundShape(EntityClickBoundShapeType type) : type(type) {}
	EntityClickBoundShapeType type;
};

struct EntityClickBoundCircle : public EntityClickBoundShape {
	EntityClickBoundCircle(float radius) : EntityClickBoundShape(EntityClickBoundShapeType::CIRCLE), radius(radius) {}
	float radius;
};

// i wanted to do runtime polymorphism here with using Entity = std::variant<...>
// but it got too complicated with the pointers... TODO: maybe try it again?
enum class EntityType {
	GOO_BALL,
	GOO_STRAND,
	ITEM_INSTANCE
};

class Entity {
	public:
		Entity(EntityType type) : type(type) {}
		virtual ~Entity() {}
		virtual void update() {}
		virtual void draw(sf::RenderWindow* window) {}
		virtual sf::Sprite getThumbnail() { return sf::Sprite(); }
		virtual std::string getDisplayName() { return ""; }
		bool wasClicked(Vector2f point);
		bool getSelected();
		void setSelected(bool selected);
		EntityType getType();
	protected:
		EntityType type;
		EntityClickBoundShape* click_bounds = nullptr;
		bool selected = false;
		Vector2f position;
		Layer layer;
		float depth;
		float rotation;

	friend class Level;
};

} // namespace gooforge

#endif // GOOFORGE_ENTITY_HH