// codeshaunted - gooforge
// source/gooforge/buffer_stream.cc
// contains BufferStream definitions
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