#pragma once

#include <type_traits>

namespace util {

template<typename T, size_t n>
struct Point {
    static_assert(n >= 2);

    std::array<T, n> x{};

    template<typename... Args>
    Point(Args... args) : x{(T)args...} { }

    template<typename S>
    Point<std::common_type_t<T, S>, n> operator+(const Point<S, n>& other) const {
        Point<std::common_type_t<T, S>, n> result{};
        for (int i = 0; i < n; i++) {
            result.x[i] = x[i] + other[i];
        }
        return result;
    }
};

}