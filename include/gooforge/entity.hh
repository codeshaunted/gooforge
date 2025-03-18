// codeshaunted - gooforge
// include/gooforge/entity.hh
// contains Entity declarations
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
	ITEM_INSTANCE,
	TERRAIN_GROUP
};

class Entity {
	public:
		Entity(EntityType type) : type(type) {}
		virtual ~Entity() {}
		virtual std::expected<void, Error> refresh() { return std::expected<void, Error>{}; }
		virtual void update() {}
		virtual void draw(sf::RenderWindow* window) {}
		virtual sf::Sprite getThumbnail() { return sf::Sprite(); }
		virtual std::string getDisplayName() { return ""; }
		bool wasClicked(Vector2f point);
		bool getSelected();
		void setSelected(bool selected);
		EntityType getType();
		Vector2f getPosition();
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