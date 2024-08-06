// codeshaunted - gooforge
// source/gooforge/error.cc
// contains Error definitions
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

#include "error.hh"

namespace gooforge {

void JSONDeserializeError::prependField(std::string field) {
	this->field = field + "." + this->field;
}

void JSONDeserializeError::prependFieldAndArrayIndex(std::string field, size_t index) {
	this->field = field + "[" + std::to_string(index) + "]." + this->field;
}

} // namespace gooforge