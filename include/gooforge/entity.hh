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

#include <memory>

#include "SFML/Graphics.hpp"

#include "vector.hh"

namespace gooforge {

class GooBall;
class GooStrand;

enum class Layer {
    BACKGROUND = 0,
    TERRAIN,
    GAMEPLAY,
    FOREGROUND,
    MISC,
    DECORATIONS
};

enum class EntityClickBoundShapeType { CIRCLE, RECTANGLE };

struct EntityClickBoundShape {
        EntityClickBoundShape(EntityClickBoundShapeType type) : type(type) {}
        EntityClickBoundShapeType type;
};

struct EntityClickBoundCircle : public EntityClickBoundShape {
        EntityClickBoundCircle(float radius)
            : EntityClickBoundShape(EntityClickBoundShapeType::CIRCLE),
              radius(radius) {}
        float radius;
};

struct EntityClickBoundRectangle : public EntityClickBoundShape {
        EntityClickBoundRectangle(Vector2f size,
                                  Vector2f pivot = Vector2f(0.5f, 0.5f))
            : EntityClickBoundShape(EntityClickBoundShapeType::RECTANGLE),
              size(size),
              pivot(pivot) {}
        Vector2f size;
        Vector2f pivot;
};

enum class EntityType {
    GOO_BALL = 0,
    GOO_STRAND,
    ITEM_INSTANCE,
    TERRAIN_GROUP
};

class Entity {
    public:
        Entity(EntityType type) : type(type) {}
        virtual ~Entity() {}
        virtual std::expected<void, Error> refresh() {
            return std::expected<void, Error>{};
        }
        virtual void update() {}
        virtual void draw(sf::RenderWindow* window) {}
        virtual sf::Sprite getThumbnail() { return sf::Sprite(); }
        virtual std::string getDisplayName() { return ""; }
        bool wasClicked(Vector2f point);
        bool getSelected();
        void setSelected(bool selected);
        void drawSelection(sf::RenderWindow* window);
        EntityType getType();
        virtual Vector2f getPosition() { return Vector2f(0.0f, 0.0f); }
        virtual float getRotation() { return 0.0f; }
        virtual float getDepth() const { return 0.0f; }
        virtual void setDepth(float depth) {}
        virtual void setPosition(Vector2f position) {}
		virtual void setRotation(float rotation) {}
        virtual void notifyAddBall(GooBall* ball) {}
        virtual void notifyRemoveBall(GooBall* ball) {}
        virtual void notifyAddStrand(GooStrand* strand) {}
        virtual void notifyRemoveStrand(GooStrand* strand) {}
        virtual void notifyUpdateBall(GooBall* ball) {}
        virtual void notifyUpdateStrand(GooStrand* strand) {}

    protected:
        EntityType type;
        EntityClickBoundShape* click_bounds = nullptr;
        bool selected = false;
        float rotation;

        friend class Level;
        friend struct EntityDepthComparator;
};

} // namespace gooforge

#endif // GOOFORGE_ENTITY_HH