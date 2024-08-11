// codeshaunted - gooforge
// source/gooforge/entity.cc
// contains Entity definitions
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

#include "entity.hh"z

namespace gooforge {

bool Entity::wasClicked(Vector2f point) {
	if (!this->click_bounds) {
		return false;
	}

	if (this->click_bounds->type == EntityClickBoundShapeType::CIRCLE) {
		EntityClickBoundCircle* circle = static_cast<EntityClickBoundCircle*>(this->click_bounds);
		if (this->position.distance(point) <= circle->radius) {
			return true;
		}
	}

	return false;
}

bool Entity::getSelected() {
	return this->selected;
}

void Entity::setSelected(bool selected) {
	this->selected = selected;
}

EntityType Entity::getType() {
	return this->type;
}

} // namespace gooforge