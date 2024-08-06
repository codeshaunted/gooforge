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

#include "SFML/Graphics.hpp"

#include "error.hh"
#include "level.hh"

namespace gooforge {

class Editor {
	public:
		void initialize();
		void update(sf::Clock delta_clock);
		void draw();
		void showSelectWOG2DirectoryDialog();
		void showErrorDialog(LegacyError error);
	private:
		sf::RenderWindow window;
		std::filesystem::path wog2_path;
		std::optional<std::shared_ptr<Error>> error = std::nullopt;
		bool error_logged = false;
		Level* level;
};

} // namespace gooforge

#endif // GOOFORGE_EDITOR_HH