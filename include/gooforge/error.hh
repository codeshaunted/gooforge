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

#include <variant>

#include "simdjson.h"

namespace gooforge {

struct BaseError {
    virtual std::string getMessage() { return ""; }
};

struct JSONDeserializeError : BaseError {
    JSONDeserializeError(std::string file_path, std::string glaze_message);
    std::string getMessage() override;
    std::string file_path;
    std::string glaze_message;
};

struct ResourceNotFoundError : BaseError {
    ResourceNotFoundError(std::string resource_id);
    std::string getMessage() override;
    std::string resource_id;
};

struct FileOpenError : BaseError {
    FileOpenError(std::string file_path);
    std::string getMessage() override;
    std::string file_path;
};

struct FileDecompressionError : BaseError {
    FileDecompressionError(std::string file_path, size_t code);
    std::string getMessage() override;
    std::string file_path;
    size_t code;
};

using Error = std::variant<JSONDeserializeError, ResourceNotFoundError, FileOpenError, FileDecompressionError>;

enum class LegacyError {
    FAILED_TO_OPEN_FILE = 0,
    RESOURCE_NOT_FOUND,
    FAILED_TO_DESERIALIZE_LEVEL,
    FAILED_TO_DESERIALIZE_VECTOR2
};

} // namespace gooforge

#endif // GOOFORGE_ERROR_HH