// codeshaunted - gooforge
// include/gooforge/error.hh
// contains Error declarations
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

#ifndef GOOFORGE_ERROR_HH
#define GOOFORGE_ERROR_HH

#include <string>
#include <variant>

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

struct XMLDeserializeError : BaseError {
        XMLDeserializeError(std::string file_path, std::string pugixml_message);
        std::string getMessage() override;
        std::string file_path;
        std::string pugixml_message;
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

struct GooBallSetupError : BaseError {
        GooBallSetupError(int uid, std::string setup_error);
        std::string getMessage() override;
        int uid;
        std::string setup_error;
};

struct LevelSetupError : BaseError {
        LevelSetupError(std::string setup_error);
        std::string getMessage() override;
        std::string setup_error;
};

using Error =
    std::variant<JSONDeserializeError, XMLDeserializeError,
                 ResourceNotFoundError, FileOpenError, FileDecompressionError,
                 GooBallSetupError, LevelSetupError>;

} // namespace gooforge

#endif // GOOFORGE_ERROR_HH