# codeshaunted - gooforge
# source/gooforge/CMakeLists.txt
# source file directory CMake file
# Copyright 2024 codeshaunted
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(GOOFORGE_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/main.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/buffer_stream.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/resource_manager.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/boy_image.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/goo_ball.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/goo_strand.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/goo_ball_template.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/editor.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/level.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/vector.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/error.cc"
	"${CMAKE_CURRENT_SOURCE_DIR}/entity.cc")

set(GOOFORGE_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/include/gooforge"
	"${CMAKE_SOURCE_DIR}/third_party"
	"${CMAKE_SOURCE_DIR}/third_party/nativefiledialog-extended/src/include"
	"${CMAKE_SOURCE_DIR}/third_party/simdjson/include"
    "${CMAKE_SOURCE_DIR}/third_party/SFML/include"
	"${CMAKE_SOURCE_DIR}/third_party/spdlog/include/spdlog"
    "${CMAKE_SOURCE_DIR}/third_party/zstd/lib"
	"${PROJECT_BINARY_DIR}/source/gooforge")

set(GOOFORGE_LINK_LIBRARIES libzstd sfml-graphics simdjson pugixml spdlog ImGui-SFML::ImGui-SFML nfd)

set(GOOFORGE_COMPILE_DEFINITIONS)

#configure_file("${CMAKE_SOURCE_DIR}/include/gooforge/config.hh.in" "config.hh")

add_executable(gooforge ${GOOFORGE_SOURCE_FILES})

target_include_directories(gooforge PUBLIC ${GOOFORGE_INCLUDE_DIRECTORIES})

target_link_libraries(gooforge PUBLIC ${GOOFORGE_LINK_LIBRARIES})

target_compile_definitions(gooforge PUBLIC ${GOOFORGE_COMPILE_DEFINITIONS})