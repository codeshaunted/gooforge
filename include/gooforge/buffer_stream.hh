// codeshaunted - gooforge
// include/gooforge/buffer_stream.hh
// contains BufferStream declarations
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