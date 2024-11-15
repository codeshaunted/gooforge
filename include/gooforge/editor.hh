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
template <typename T>
struct ModifyPropertyEditorAction;

// this template shit is horrendous but whatever
using EditorAction = std::variant<SelectEditorAction, DeselectEditorAction, ModifyPropertyEditorAction<GooBallType>, ModifyPropertyEditorAction<float>>;

struct BaseEditorAction {
	// we define implicit actions as actions executed before the main action is to be executed
	// this is needed for stuff like selection, where we want to deselect all when a user selects
	// a single entity in the editor window
	BaseEditorAction(std::vector<EditorAction> implicit_actions) : implicit_actions(implicit_actions) {}
	virtual std::expected<void, Error> execute(Editor* editor) { return std::expected<void, Error>{}; }
	virtual std::expected<void, Error> revert(Editor* editor) { return std::expected<void, Error>{}; }
	std::vector<EditorAction> implicit_actions;
};

struct SelectEditorAction : public BaseEditorAction {
	SelectEditorAction(std::vector<Entity*> entities, std::vector<EditorAction> implicit_actions = {}) : BaseEditorAction(implicit_actions), entities(entities) {}
	std::expected<void, Error> execute(Editor* editor) override;
	std::expected<void, Error> revert(Editor* editor) override;
	std::vector<Entity*> entities;
};

struct DeselectEditorAction : public BaseEditorAction {
	DeselectEditorAction(std::vector<Entity*> entities, std::vector<EditorAction> implicit_actions = {}) : BaseEditorAction(implicit_actions), entities(entities) {}
	std::expected<void, Error> execute(Editor* editor) override;
	std::expected<void, Error> revert(Editor* editor) override;
	std::vector<Entity*> entities;
};

template<typename T>
struct ModifyPropertyEditorAction : public BaseEditorAction {
	ModifyPropertyEditorAction(T* property, T new_value, Entity* refresh_entity = nullptr) : BaseEditorAction({}), property(property), old_value(*property), new_value(new_value), refresh_entity(refresh_entity) {}
	std::expected<void, Error> execute(Editor* editor) override {
		*this->property = new_value;
		if (this->refresh_entity) {
			auto result = this->refresh_entity->refresh();
			if (!result) {
				return std::unexpected(result.error());
			}
		}

		return std::expected<void, Error>{};
	}
	std::expected<void, Error> revert(Editor* editor) override {
		*this->property = old_value;
		
		if (this->refresh_entity) {
			auto result = this->refresh_entity->refresh();
			if (!result) {
				return std::unexpected(result.error());
			}
		}

		return std::expected<void, Error>{};
	}
	T* property;
	T old_value;
	T new_value;
	Entity* refresh_entity;
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
		std::vector<Error> errors;
		Level* level = nullptr;
		std::string level_file_path;
		std::vector<Entity*> selected_entities;
		std::deque<EditorAction> undo_stack;
		sf::Clock undo_clock;
		sf::Time undo_cooldown = sf::milliseconds(200);
		std::deque<EditorAction> redo_stack;
		void update(sf::Clock& delta_clock);
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
		bool registerGooBallTypeCombo(const char* label, GooBallType* type, GooBall* refresh_goo_ball = nullptr);

	// we either make everything public or declare every
	// single derived action as a friend class, pick
	// your poison, or maybe just structure it better? :P
	friend struct SelectEditorAction;
	friend struct DeselectEditorAction;
};

} // namespace gooforge

#endif // GOOFORGE_EDITOR_HH