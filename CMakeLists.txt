# codeshaunted - gooforge
# CMakeLists.txt
# root CMake file
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

cmake_minimum_required(VERSION 3.12)

project(gooforge VERSION 1.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# for some reason using target_compile_options for this was not working
# this is required for glaze on MSVC because Microsoft cannot make their
# standards compliant preprocessor the default for some reason
if(MSVC)
	add_compile_options("/Zc:preprocessor")
endif()

# third party
add_subdirectory("third_party")

# internal
add_subdirectory("source")