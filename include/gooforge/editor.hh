// codeshaunted - gooforge
// include/gooforge/editor.hh
// contains Editor declarations
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

#ifndef GOOFORGE_EDITOR_HH
#define GOOFORGE_EDITOR_HH

#include <deque>
#include <filesystem>
#include <functional>
#include <variant>

#include "SFML/Graphics.hpp"

#include "error.hh"
#include "level.hh"

namespace gooforge {

class Editor;

struct EditorAction {
        // we define implicit actions as actions executed before the main action
        // is to be executed this is needed for stuff like selection, where we
        // want to deselect all when a user selects a single entity in the
        // editor window
        virtual ~EditorAction();
        EditorAction(std::vector<EditorAction*> implicit_actions)
            : implicit_actions(implicit_actions) {}
        virtual std::expected<void, Error> execute(Editor* editor) {
            return std::expected<void, Error>{};
        }
        virtual std::expected<void, Error> revert(Editor* editor) {
            return std::expected<void, Error>{};
        }
        std::vector<EditorAction*> implicit_actions;
};

struct SelectEditorAction : public EditorAction {
        SelectEditorAction(std::vector<Entity*> entities,
                           std::vector<EditorAction*> implicit_actions = {})
            : EditorAction(implicit_actions), entities(entities) {}
        std::expected<void, Error> execute(Editor* editor) override;
        std::expected<void, Error> revert(Editor* editor) override;
        std::vector<Entity*> entities;
};

struct DeselectEditorAction : public EditorAction {
        DeselectEditorAction(std::vector<Entity*> entities,
                             std::vector<EditorAction*> implicit_actions = {})
            : EditorAction(implicit_actions), entities(entities) {}
        std::expected<void, Error> execute(Editor* editor) override;
        std::expected<void, Error> revert(Editor* editor) override;
        std::vector<Entity*> entities;
};

struct DeleteEditorAction : public EditorAction {
        ~DeleteEditorAction();
        DeleteEditorAction(std::vector<Entity*> entities,
                           std::vector<EditorAction*> implicit_actions = {})
            : EditorAction(implicit_actions), entities(entities) {}
        std::expected<void, Error> execute(Editor* editor) override;
        std::expected<void, Error> revert(Editor* editor) override;
        std::vector<Entity*> entities;
        bool reverted = false;
};

template <typename T>
struct ModifyPropertyEditorAction : public EditorAction {
        ModifyPropertyEditorAction(std::function<T()> get,
                                   std::function<void(T)> set, T new_value,
                                   std::vector<EditorAction*> implicit_actions = {})
            : EditorAction(implicit_actions), get(get), set(set), new_value(new_value) {}
        std::expected<void, Error> execute(Editor* editor) override {
            this->original_value = this->get();
            this->set(this->new_value);

            return std::expected<void, Error>{};
        }
        std::expected<void, Error> revert(Editor* editor) override {
            this->set(this->original_value);

            return std::expected<void, Error>{};
        }
        std::function<T()> get;
        std::function<void(T)> set;
        T new_value;
        T original_value;
};

enum class EditorToolType { MOVE = 0, SCALE };

struct DefaultPropertyTag {};
struct ItemTemplatePropertyTag {};

class Editor {
    public:
        ~Editor();
        void initialize();

    private:
        static std::unordered_map<EditorToolType, const char*>
            tool_type_to_name;
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
        std::deque<EditorAction*> undo_stack;
        sf::Clock undo_clock;
        sf::Time undo_cooldown = sf::milliseconds(200);
        std::deque<EditorAction*> redo_stack;
        EditorToolType selected_tool = EditorToolType::MOVE;
        int undo_depth = 50;
        void update(sf::Clock& delta_clock);
        void draw();
        void processEvents();
        void doAction(EditorAction* action);
        void undoAction(EditorAction* action);
        void redoAction(EditorAction* action);
        void undoLastAction();
        void redoLastUndo();
        void clearUndos();
        void clearRedos();
        void doEntitySelection(Entity* entity);
        void doEntitiesDeletion(std::vector<Entity*> entities);
        void doOpenFile();
        void doCloseFile();
        void registerMainMenuBar();
        void registerErrorDialog();
        void showErrorDialog();
        void registerSelectWOG2DirectoryDialog();
        void showSelectWOG2DirectoryDialog();
        void registerLevelWindow();
        void registerPropertiesWindow();
        void registerResourcesWindow();
        void registerToolbarWindow();
        bool registerGooBallTypeCombo(const char* label, GooBallType* type);
        template <typename T, typename Tag = DefaultPropertyTag>
        void registerPropertiesField(const char* label, std::function<T()> get,
            std::function<void(T)> set,
            std::vector<EditorAction*> implicit_actions = {});

        // we either make everything public or declare every
        // single derived action as a friend class, pick
        // your poison, or maybe just structure it better? :P
        friend struct SelectEditorAction;
        friend struct DeselectEditorAction;
        friend struct DeleteEditorAction;
};

} // namespace gooforge

#endif // GOOFORGE_EDITOR_HH