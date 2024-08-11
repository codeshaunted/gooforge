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

// forward declare these so we can use them in the variant
// and then use the variant in BaseEditorAction
struct SelectEditorAction;
struct DeselectEditorAction;

using EditorAction = std::variant<SelectEditorAction, DeselectEditorAction>;

struct BaseEditorAction {
	// we define implicit actions as actions executed before the main action is to be executed
	// this is needed for stuff like selection, where we want to deselect all when a user selects
	// a single entity in the editor window
	BaseEditorAction(std::vector<EditorAction> implicit_actions) : implicit_actions(implicit_actions) {}
	virtual void execute(Editor* editor) {}
	virtual void revert(Editor* editor) {}
	std::vector<EditorAction> implicit_actions;
};

struct SelectEditorAction : public BaseEditorAction {
	SelectEditorAction(std::vector<Entity*> entities, std::vector<EditorAction> implicit_actions = {}) : BaseEditorAction(implicit_actions), entities(entities) {}
	void execute(Editor* editor) override;
	void revert(Editor* editor) override;
	std::vector<Entity*> entities;
};

struct DeselectEditorAction : public BaseEditorAction {
	DeselectEditorAction(std::vector<Entity*> entities, std::vector<EditorAction> implicit_actions = {}) : BaseEditorAction(implicit_actions), entities(entities) {}
	void execute(Editor* editor) override;
	void revert(Editor* editor) override;
	std::vector<Entity*> entities;
};

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
		sf::Clock undo_clock;
		sf::Time undo_cooldown = sf::milliseconds(200);
		std::deque<EditorAction> redo_stack;
		void update(sf::Clock delta_clock);
		void draw();
		void processEvents();
		void doAction(EditorAction action);
		void undoAction(EditorAction action);
		void undoLastAction();
		void redoLastUndo();
		void doEntitySelection(Entity* entity);
		void doOpenFile();
		void doCloseFile();
		void registerMainMenuBar();
		void registerErrorDialog();
		void showErrorDialog();
		void registerSelectWOG2DirectoryDialog();
		void showSelectWOG2DirectoryDialog();
		void registerLevelWindow();
		void registerPropertiesWindow();

	// we either make everything public or declare every
	// single derived action as a friend class, pick
	// your poison, or maybe just structure it better? :P
	friend class SelectEditorAction;
	friend class DeselectEditorAction;
};

} // namespace gooforge

#endif // GOOFORGE_EDITOR_HH