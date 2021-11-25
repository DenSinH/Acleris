#pragma once

#include "VMath/Vector.h"
#include "util/Vector.h"

#include <array>
#include <tuple>


template<size_t n, typename... Args>
struct Vertex {
    static constexpr size_t dim = n;
    static constexpr size_t no_args = sizeof...(Args);

    vmath::Vector<float, n> x{};
    const std::tuple<Args...> args;

    Vertex(vmath::Vector<float, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }
};

template<int n, typename... Args>
Vertex<n, Args...> MakeVertex(vmath::Vector<float, n> x, Args... args) {
    return Vertex<n, Args...>(std::move(x), args...);
}

template<typename S, size_t n, typename... Args>
Vertex<n, Args...> operator+(const Vertex<n, Args...>& vtx, const vmath::Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<n, Args...>(vtx.x + v, args...);}, vtx.args);
}

template<typename S, size_t n, typename... Args>
Vertex<n, Args...> operator+(const vmath::Vector<S, n>& v, const Vertex<n, Args...>& vtx) {
    return vtx + v;
}

template<typename S, size_t n, typename... Args>
Vertex<n, Args...> operator-(const Vertex<n, Args...>& vtx, const vmath::Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<n, Args...>(vtx.x - v, args...);}, vtx.args);
}