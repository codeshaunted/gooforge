# codeshaunted - gooforge
# CMakeLists.txt
# root CMake file
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