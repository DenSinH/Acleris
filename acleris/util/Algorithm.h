#pragma once

#include <algorithm>

namespace util {

template<typename T>
consteval T min(T n) { return n; }
template<typename T, typename... Ts>
consteval T min(T n, T m, Ts... args) { return min<Ts...>(std::min(n, m), args...); }

template<typename T>
constexpr bool in_range(T val, T min, T max) {
    return std::clamp<T>(val, min, max) == val;
}

}