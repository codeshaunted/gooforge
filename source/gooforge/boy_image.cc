// codeshaunted - gooforge
// source/gooforge/boy_image.cc
// contains BoyImage definitions
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

#include "boy_image.hh"

#include <fstream>

#include "zstd.h"

#include "buffer_stream.hh"

namespace gooforge {

std::expected<sf::Image, Error> BoyImage::loadFromFile(std::string_view path) {
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    if (!file) {
        return std::unexpected(FileOpenError(std::string(path)));
    }

    size_t file_size = file.tellg();

    file.seekg(36); // skip header
    size_t compressed_data_size = file_size - 36;
    char* compressed_data = new char[compressed_data_size];
    file.read(compressed_data, compressed_data_size);

    file.close();

    size_t decompressed_size = ZSTD_getFrameContentSize(compressed_data, compressed_data_size);
    if (ZSTD_isError(decompressed_size)) {
        delete[] compressed_data;
        return std::unexpected(FileDecompressionError(std::string(path), decompressed_size));
    }

    // some files wont decompress with the getFrameContentSize result...
    // for now just do decompressed_size * 2, TODO: switch to stream decompression
    uint8_t* decompressed_data = new uint8_t[decompressed_size * 2];
    size_t result = ZSTD_decompress(decompressed_data, decompressed_size * 2, compressed_data, compressed_data_size);
    if (ZSTD_isError(result)) {
        delete[] compressed_data;
        delete[] decompressed_data;
        return std::unexpected(FileDecompressionError(std::string(path), result));
    }

    delete[] compressed_data;

    BufferStream stream(reinterpret_cast<char*>(decompressed_data), decompressed_size);
    stream.seek(36); // skip first part of header

    uint32_t pixel_width = stream.read<uint32_t>();
    uint32_t pixel_height = stream.read<uint32_t>();
    stream.seek(24); // skip the rest of the header

    char* pixel_data = stream.remainder();

    sf::Image image;
    image.create(pixel_width, pixel_height, (uint8_t*)pixel_data);

    delete[] decompressed_data;

    return image;
}

} // namespace gooforge