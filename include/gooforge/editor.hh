// codeshaunted - gooforge
// include/gooforge/editor.hh
// contains Editor declarations
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

#ifndef GOOFORGE_EDITOR_HH
#define GOOFORGE_EDITOR_HH

#include <filesystem>
#include <deque>
#include <variant>

#include "SFML/Graphics.hpp"

#include "error.hh"
#include "level.hh"

namespace gooforge {

struct BaseEditorAction {
	BaseEditorAction(bool implicit) : implicit(implicit) {}
	bool implicit = false;
};

struct SelectEditorAction : public BaseEditorAction {
	SelectEditorAction(std::vector<Entity*> entities, bool implicit = false) : BaseEditorAction(implicit), entities(entities) {}
	std::vector<Entity*> entities;
};

struct DeselectEditorAction : public BaseEditorAction {
	DeselectEditorAction(std::vector<Entity*> entities, bool implicit = false) : BaseEditorAction(implicit), entities(entities) {}
	std::vector<Entity*> entities;
};

using EditorAction = std::variant<SelectEditorAction, DeselectEditorAction>;

class Editor {
	public:
		~Editor();
		void initialize();
	private:
		sf::RenderWindow window;
		sf::View view;
		float zoom = 1.0f;
		bool panning = false;
		sf::Vector2f pan_start_position;
		std::filesystem::path wog2_path;
		std::optional<Error> error = std::nullopt;
		Level* level = nullptr;
		std::vector<Entity*> selected_entities;
		std::deque<EditorAction> undo_stack;
		size_t undo_stack_count;
		std::deque<EditorAction> redo_stack;
		size_t redo_stack_count;
		void update(sf::Clock delta_clock);
		void draw();
		void processEvents();
		void doAction(EditorAction action, bool for_redo = false);
		void undoAction(EditorAction action);
		void undoLastAction();
		void redoLastUndo();
		void doEntitySelection(Entity* entity);
		void registerMainMenuBar();
		void registerErrorDialog();
		void showErrorDialog();
		void registerSelectWOG2DirectoryDialog();
		void showSelectWOG2DirectoryDialog();
		void registerLevelWindow();
};

} // namespace gooforge

#endif // GOOFORGE_EDITOR_HH