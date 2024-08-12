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

JSONDeserializeError::JSONDeserializeError(std::string file_path, std::string glaze_message) {
	this->file_path = file_path;
	this->glaze_message = glaze_message;
	spdlog::error(this->getMessage());
}

std::string JSONDeserializeError::getMessage() {
	return "Failed to deserialize JSON file '" + this->file_path + "', with error '" + this->glaze_message + "'";
}

XMLDeserializeError::XMLDeserializeError(std::string file_path, std::string pugixml_message) {
	this->file_path = file_path;
	this->pugixml_message = pugixml_message;
	spdlog::error(this->getMessage());
}

std::string XMLDeserializeError::getMessage() {
	return "Failed to deserialize XML file '" + this->file_path + "', with error '" + this->pugixml_message + "'";
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

GooBallSetupError::GooBallSetupError(int uid, std::string setup_error) {
	this->uid = uid;
	this->setup_error = setup_error;
	spdlog::error(this->getMessage());
}

std::string GooBallSetupError::getMessage() {
	return "Failed to setup GooBall with uid '" + std::to_string(this->uid) + "', with error '" + this->setup_error + "'";
}

LevelSetupError::LevelSetupError(std::string setup_error) {
	this->setup_error = setup_error;
	spdlog::error(this->getMessage());
}

std::string LevelSetupError::getMessage() {
	return "Failed to setup GooBall with error '" + this->setup_error + "'";
}

} // namespace gooforge