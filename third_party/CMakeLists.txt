# codeshaunted - gooforge
# third_party/CMakeLists.txt
# third party CMake file
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

# simdjson
add_subdirectory("simdjson")

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