# codeshaunted - gooforge
# source/gooforge/CMakeLists.txt
# source file directory CMake file
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

set(GOOFORGE_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/buffer_stream.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/resource_manager.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/boy_image.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/goo_ball.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/goo_strand.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/editor.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/level.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/error.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/entity.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/item.cc")

set(GOOFORGE_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/include/gooforge"
	"${CMAKE_SOURCE_DIR}/third_party"
	"${CMAKE_SOURCE_DIR}/third_party/nativefiledialog-extended/src/include"
    "${CMAKE_SOURCE_DIR}/third_party/SFML/include"
	"${CMAKE_SOURCE_DIR}/third_party/spdlog/include/spdlog"
    "${CMAKE_SOURCE_DIR}/third_party/zstd/lib"
	"${CMAKE_SOURCE_DIR}/third_party/glaze/include"
	"${PROJECT_BINARY_DIR}/source/gooforge")

set(GOOFORGE_LINK_LIBRARIES libzstd sfml-graphics pugixml spdlog ImGui-SFML::ImGui-SFML nfd)

set(GOOFORGE_COMPILE_DEFINITIONS)

#configure_file("${CMAKE_SOURCE_DIR}/include/gooforge/config.hh.in" "config.hh")

add_executable(gooforge ${GOOFORGE_SOURCE_FILES})

target_include_directories(gooforge PUBLIC ${GOOFORGE_INCLUDE_DIRECTORIES})

target_link_libraries(gooforge PUBLIC ${GOOFORGE_LINK_LIBRARIES})

target_compile_definitions(gooforge PUBLIC ${GOOFORGE_COMPILE_DEFINITIONS})