// codeshaunted - gooforge
// source/gooforge/error.cc
// contains Error definitions
// Copyright (C) 2024 codeshaunted
//
// This file is part of gooforge.
// gooforge is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// gooforge is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gooforge. If not, see <https://www.gnu.org/licenses/>.

#include "error.hh"

#include "spdlog.h"
#include "zstd.h"

namespace gooforge {

JSONDeserializeError::JSONDeserializeError(std::string file_path,
                                           std::string glaze_message) {
    this->file_path = file_path;
    this->glaze_message = glaze_message;
    spdlog::error(this->getMessage());
}

std::string JSONDeserializeError::getMessage() {
    return "Failed to deserialize JSON file '" + this->file_path +
           "', with error '" + this->glaze_message + "'";
}

XMLDeserializeError::XMLDeserializeError(std::string file_path,
                                         std::string pugixml_message) {
    this->file_path = file_path;
    this->pugixml_message = pugixml_message;
    spdlog::error(this->getMessage());
}

std::string XMLDeserializeError::getMessage() {
    return "Failed to deserialize XML file '" + this->file_path +
           "', with error '" + this->pugixml_message + "'";
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

FileDecompressionError::FileDecompressionError(std::string file_path,
                                               size_t code) {
    this->file_path = file_path;
    this->code = code;
    spdlog::error(this->getMessage());
}

std::string FileDecompressionError::getMessage() {
    return "Failed to decompress file at path '" + this->file_path +
           "', with error '" + ZSTD_getErrorName(this->code) + "'";
}

GooBallSetupError::GooBallSetupError(int uid, std::string setup_error) {
    this->uid = uid;
    this->setup_error = setup_error;
    spdlog::error(this->getMessage());
}

std::string GooBallSetupError::getMessage() {
    return "Failed to setup GooBall with uid '" + std::to_string(this->uid) +
           "', with error '" + this->setup_error + "'";
}

LevelSetupError::LevelSetupError(std::string setup_error) {
    this->setup_error = setup_error;
    spdlog::error(this->getMessage());
}

std::string LevelSetupError::getMessage() {
    return "Failed to setup GooBall with error '" + this->setup_error + "'";
}

}  // namespace gooforge