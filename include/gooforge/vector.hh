// codeshaunted - gooforge
// include/gooforge/vector.hh
// contains Vector declarations
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

#ifndef GOOFORGE_VECTOR_HH
#define GOOFORGE_VECTOR_HH

#include <expected>

#include "simdjson.h"

#include "error.hh"

namespace gooforge {

template<typename T, size_t N>
class Vector {
    public:
        Vector();
        Vector(std::initializer_list<T> data);
        T& get(size_t i);
        T& operator[](size_t i);
        T length();
        T dot(Vector<T, N>& other);
        Vector<T, N> scale(T scalar);
    protected:
        T data[N];
};

template<typename T, size_t N>
Vector<T, N>::Vector() {
    static_assert(std::is_arithmetic_v<T>, "'T' must be an arithmetic type");

    if (N <= 0) {
        throw std::invalid_argument("Invalid number of elements");
    }

    for (size_t j = 0; j < N; ++j) {
        this->data[j] = 0;
    }
}

template<typename T, size_t N>
Vector<T, N>::Vector(std::initializer_list<T> data) {
    if (data.size() != N) {
        throw std::invalid_argument("Invalid number of elements");
    }

    size_t i = 0;
    for (const auto& element : data) {
        this->data[i] = element;
        ++i;
    }
}

template<typename T, size_t N>
T& Vector<T, N>::get(size_t i) {
    if (i < 0 || i >= N) {
        throw std::out_of_range("Value for 'i' is out of range");
    }

    return this->data[i];
}

template<typename T, size_t N>
T& Vector<T, N>::operator[](size_t i) {
    return this->get(i);
}

template <typename T, size_t N>
T Vector<T, N>::length() {
    T squares_sum = 0;
    for (size_t i = 0; i < N; ++i) {
        squares_sum += this->data[i] * this->data[i];
    }

    return std::sqrt(squares_sum);
}

template<typename T, size_t N>
T Vector<T, N>::dot(Vector<T, N>& other) {
    T product = 0;

    for (size_t i = 0; i < N; ++i) {
        product += this->data[i] * other.data[i];
    }

    return product;
}

template<typename T, size_t N>
Vector<T, N> Vector<T, N>::scale(T scalar) {
    Vector<T, N> result;

    for (size_t i = 0; i < N; ++i) {
        result.data[i] = this->data[i] * scalar;
    }

    return result;
}


template<typename T>
class Vector2 : public Vector<T, 2> {
    public:
        T& x = Vector<T, 2>::data[0];
        T& y = Vector<T, 2>::data[1];
        Vector2() : Vector<T, 2>() {}
        Vector2(T _x, T _y) : Vector<T, 2>({ _x, _y }) {}
        Vector2(const Vector<T, 2>& other) : Vector<T, 2>(other) {}
        Vector2<T>& operator=(const Vector2<T>& other);
};

template<typename T>
Vector2<T>& Vector2<T>::operator=(const Vector2<T>& other) {
    if (this != &other) {
        Vector<T, 2>::operator=(other);
    }

    return *this;
}

class Vector2f : public Vector2<float> {
    public:
        Vector2f() : Vector2<float>() {}
        Vector2f(const Vector<float, 2>& other) : Vector2<float>(other) {}
        static std::expected<Vector2f, std::shared_ptr<JSONDeserializeError>> deserialize(simdjson::ondemand::value json);
};

typedef Vector2<int> Vector2i;

} // namespace gooforge

#endif // GOOFORGE_VECTOR_HH