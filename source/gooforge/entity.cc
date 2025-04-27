// codeshaunted - gooforge
// source/gooforge/entity.cc
// contains Entity definitions
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

#include "entity.hh"

#include <cmath>

#include "constants.hh"
#include "level.hh"

namespace gooforge {

bool Entity::wasClicked(Vector2f point) {
    if (!this->click_bounds) {
        return false;
    }

    if (this->click_bounds->type == EntityClickBoundShapeType::CIRCLE) {
        EntityClickBoundCircle* circle =
            static_cast<EntityClickBoundCircle*>(this->click_bounds);
        if (this->getPosition().distance(point) <= circle->radius) {
            return true;
        }
    } else if (this->click_bounds->type ==
               EntityClickBoundShapeType::RECTANGLE) {
        EntityClickBoundRectangle* rectangle =
            static_cast<EntityClickBoundRectangle*>(this->click_bounds);
        Vector2f pos = this->getPosition();
        float rot = this->getRotation();

        Vector2f local = point - pos;

        float cosine = std::cos(-rot);
        float sine = std::sin(-rot);
        Vector2f rotated_local(local.x * cosine - local.y * sine,
                               local.x * sine + local.y * cosine);

        Vector2f pivot_offset =
            Vector2f((rectangle->pivot.x - 0.5f) * rectangle->size.x,
                     (rectangle->pivot.y - 0.5f) * rectangle->size.y);

        Vector2f adjusted_local = rotated_local + pivot_offset;

        if (std::abs(adjusted_local.x) <= rectangle->size.x / 2.0f &&
            std::abs(adjusted_local.y) <= rectangle->size.y / 2.0f) {
            return true;
        }
    }

    return false;
}

bool Entity::getSelected() { return this->selected; }

void Entity::setSelected(bool selected) { this->selected = selected; }

void Entity::drawSelection(sf::RenderWindow* window) {
    if (!this->click_bounds) return;

    if (this->click_bounds->type == EntityClickBoundShapeType::CIRCLE) {
        EntityClickBoundCircle* circle =
            static_cast<EntityClickBoundCircle*>(this->click_bounds);
        float screen_radius = circle->radius * GOOFORGE_PIXELS_PER_UNIT;

        sf::CircleShape shape(screen_radius);
        shape.setOrigin(screen_radius, screen_radius);
        shape.setPosition(Level::worldToScreen(this->getPosition()));
        shape.setOutlineThickness(2.0f);
        shape.setOutlineColor(sf::Color::Blue);
        shape.setFillColor(sf::Color::Transparent);

        window->draw(shape);
    } else if (this->click_bounds->type ==
               EntityClickBoundShapeType::RECTANGLE) {
        EntityClickBoundRectangle* rectangle =
            static_cast<EntityClickBoundRectangle*>(this->click_bounds);

        sf::RectangleShape shape;
        shape.setSize(
            sf::Vector2f(rectangle->size.x * GOOFORGE_PIXELS_PER_UNIT,
                         rectangle->size.y * GOOFORGE_PIXELS_PER_UNIT));
        shape.setOrigin(
            shape.getSize().x * rectangle->pivot.x,
            shape.getSize().y - (shape.getSize().y * rectangle->pivot.y));
        shape.setPosition(Level::worldToScreen(this->getPosition()));
        shape.setRotation(-Level::radiansToDegrees(this->getRotation()));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color::Blue);
        shape.setOutlineThickness(2.0f);

        window->draw(shape);
    }
}

EntityType Entity::getType() const { return this->type; }

} // namespace gooforge