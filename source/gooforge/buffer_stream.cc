// codeshaunted - gooforge
// source/gooforge/buffer_stream.cc
// contains BufferStream definitions
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

#include "buffer_stream.hh"

namespace gooforge {

void BufferStream::seek(size_t seek_index, bool absolute) {
    size_t absolute_index = absolute ? seek_index : this->index + seek_index;

    if (absolute_index >= this->buffer_size) {
        throw std::out_of_range("BufferStream::seek out of range");
    }

    this->index = absolute_index;
}

char* BufferStream::remainder() {
    return this->buffer + this->index;
}

} // namespace gooforge