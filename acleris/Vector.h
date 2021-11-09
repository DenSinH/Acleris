#pragma once

#include <array>

template<typename T, size_t n>
struct Vector {
    static_assert(n >= 2);

    std::array<T, n> x{};

    template<typename... Args>
    Vector(Args... args) : x{(T)args...} { }
    template<typename... Args>

    Vector(std::array<T, n> x) : x{std::move(x)} { }

    template<typename S>
    Vector<T, n> operator*(const S& other) const {
        Vector<T, n> result;
        for (int i = 0; i < n; i++) {
            result.x[i] = x[i] * other;
        }
        return result;
    }

    template<typename S>
    Vector<T, n> operator+(const Vector<S, n>& other) const {
        Vector<T, n> result;
        for (int i = 0; i < n; i++) {
            result.x[i] = x[i] + other[i];
        }
        return result;
    }

    template<typename S>
    Vector<T, n> operator-(const Vector<S, n>& other) const {
        Vector<T, n> result;
        for (int i = 0; i < n; i++) {
            result.x[i] = x[i] - other[i];
        }
        return result;
    }

    T& operator[](size_t index) {
        return x[index];
    }

    const T& operator[](size_t index) const {
        return x[index];
    }
};


template<typename T, size_t n, typename S>
Vector<T, n> operator*(const S& other, const Vector<T, n>& v) {
    return v * other;
}