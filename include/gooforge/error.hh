// codeshaunted - gooforge
// include/gooforge/error.hh
// contains Error declarations
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

#ifndef GOOFORGE_ERROR_HH
#define GOOFORGE_ERROR_HH

#include "simdjson.h"

namespace gooforge {

enum class ErrorType {
    JSON_DESERIALIZE = 0,
};

struct Error {
    Error(ErrorType type) : type(type) {}
    ErrorType type;
};

struct JSONDeserializeError : public Error {
    JSONDeserializeError(std::string field, simdjson::error_code code) : Error(ErrorType::JSON_DESERIALIZE), field(field), code(code) {}
    JSONDeserializeError(std::string field, size_t array_index, simdjson::error_code code) : Error(ErrorType::JSON_DESERIALIZE), field(field + "[" + std::to_string(array_index) + "]"), code(code) {}
    void prependField(std::string field);
    void prependFieldAndArrayIndex(std::string field, size_t index);
    std::string field;
    simdjson::error_code code;
};

enum class LegacyError {
    FAILED_TO_OPEN_FILE = 0,
    RESOURCE_NOT_FOUND,
    FAILED_TO_DESERIALIZE_LEVEL,
    FAILED_TO_DESERIALIZE_VECTOR2
};

} // namespace gooforge

#endif // GOOFORGE_ERROR_HH