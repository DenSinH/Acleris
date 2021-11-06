#pragma once

namespace util {

template<typename T>
consteval T min(T n) { return n; }
template<typename T, typename... Ts>
consteval T min(T n, T m, Ts... args) { return min<Ts...>(std::min(n, m), args...); }

}