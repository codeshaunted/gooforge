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

#include "spdlog.h"
#include "zstd.h"

namespace gooforge {

void JSONDeserializeError::prependField(std::string field) {
	this->field = field + "." + this->field;
}

void JSONDeserializeError::prependFieldAndArrayIndex(std::string field, size_t index) {
	this->field = field + "[" + std::to_string(index) + "]." + this->field;
}

void JSONDeserializeError::prependFilePath(std::string file_path) {
	this->file_path = file_path;
	spdlog::error(this->getMessage()); // log here because this is the termination of possible error stacking
}

std::string JSONDeserializeError::getMessage() {
	if (file_path.empty()) {
		return "Failed to deserialize JSON at field '" + this->field + "', with error '" + simdjson::error_message(this->code) + "'";
	} else {
		return "Failed to deserialize JSON file '" + this->file_path + "' at field '" + this->field + "', with error '" + simdjson::error_message(this->code) + "'";
	}
}

ResourceNotFoundError::ResourceNotFoundError(std::string resource_id) {
	this->resource_id = resource_id;
	spdlog::error(this->getMessage());
}

std::string ResourceNotFoundError::getMessage() {
	return "Failed to find resource with id '" + this->resource_id + "'";
}

FileOpenError::FileOpenError(std::string file_path) {
	this->file_path = file_path;
	spdlog::error(this->getMessage());
}

std::string FileOpenError::getMessage() {
	return "Failed to open file at path '" + this->file_path + "'";
}

FileDecompressionError::FileDecompressionError(std::string file_path, size_t code) {
	this->file_path = file_path;
	this->code = code;
	spdlog::error(this->getMessage());
}

std::string FileDecompressionError::getMessage() {
	return "Failed to decompress file at path '" + this->file_path + "', with error '" + ZSTD_getErrorName(this->code) + "'";
}

} // namespace gooforge