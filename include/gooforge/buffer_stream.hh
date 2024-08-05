// codeshaunted - gooforge
// include/gooforge/buffer_stream.hh
// contains BufferStream declarations
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

#ifndef GOOFORGE_BUFFER_STREAM_HH
#define GOOFORGE_BUFFER_STREAM_HH

#include <stdexcept>

namespace gooforge {

class BufferStream {
    public:
        BufferStream(char* buffer, size_t buffer_size) : buffer(buffer), buffer_size(buffer_size), index(0) {}
        template<typename T>
        T read();
        void seek(size_t seek_index, bool absolute = false);
        char* remainder();
    private:
        char* buffer;
        size_t buffer_size;
        size_t index;
};

template<typename T>
T BufferStream::read() {
    if (this->index + sizeof(T) > buffer_size) {
        throw std::out_of_range("BufferStream::read out of range");
    }

    T value;
    std::memcpy(&value, this->buffer + this->index, sizeof(T));
    this->index += sizeof(T);

    return value;
}

} // namespace gooforge

#endif // GOOFORGE_BUFFER_STREAM_HH