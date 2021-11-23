#pragma once

#include "VMath/Vector.h"

#include <array>
#include <tuple>


template<typename T, size_t n, typename... Args>
struct Vertex {
    static constexpr size_t dim = n;
    static constexpr size_t no_args = sizeof...(Args);
    using type = T;

    vmath::Vector<T, n> x{};
    const std::tuple<Args...> args;

    Vertex(vmath::Vector<T, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }
};

template<typename T, int n, typename... Args>
Vertex<T, n, Args...> MakeVertex(vmath::Vector<T, n> x, Args... args) {
    return Vertex<T, n, Args...>(std::move(x), args...);
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator+(const Vertex<T, n, Args...>& vtx, const vmath::Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<T, n, Args...>(vtx.x + v, args...);}, vtx.args);
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator+(const vmath::Vector<S, n>& v, const Vertex<T, n, Args...>& vtx) {
    return vtx + v;
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator-(const Vertex<T, n, Args...>& vtx, const vmath::Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<T, n, Args...>(vtx.x - v, args...);}, vtx.args);
}
