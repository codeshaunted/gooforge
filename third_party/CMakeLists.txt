# codeshaunted - gooforge
# third_party/CMakeLists.txt
# third party CMake file
# Copyright (C) 2024 codeshaunted
#
# This file is part of gooforge.
# gooforge is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# gooforge is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with gooforge. If not, see <https://www.gnu.org/licenses/>.

# glaze
add_subdirectory("glaze")

# imgui-sfml
set(IMGUI_SFML_FIND_SFML FALSE)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/imgui")
add_subdirectory("imgui-sfml")

# nativefiledialog-extended
add_subdirectory("nativefiledialog-extended")

# pugixml
add_subdirectory("pugixml")

# SFML
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("SFML")

# SFML
add_subdirectory("spdlog")

# zstd
set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_LEGACY_SUPPORT OFF)
set(ZSTD_BUILD_SHARED OFF)
add_subdirectory("zstd/build/cmake")