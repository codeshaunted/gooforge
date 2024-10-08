// codeshaunted - gooforge
// source/gooforge/boy_image.cc
// contains BoyImage definitions
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