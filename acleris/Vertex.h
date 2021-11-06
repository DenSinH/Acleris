#pragma once

#include "Vector.h"

#include <array>
#include <tuple>


template<typename T, size_t n, typename... Args>
struct Vertex {
    static constexpr size_t dim = n;
    static constexpr size_t no_args = sizeof...(Args);
    using type = T;

    Vector<T, n> x{};
    const std::tuple<Args...> args;

    Vertex(std::array<T, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }

    Vertex(Vector<T, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }
};

template<typename T, int n, typename... Args>
Vertex<T, n, Args...> MakeVertex(std::array<T, n> x, Args... args) {
    return Vertex<T, n, Args...>(std::move(x), args...);
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator+(const Vertex<T, n, Args...>& vtx, const Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<T, n, Args...>((vtx.x + v).x, args...);}, vtx.args);
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator+(const Vector<S, n>& v, const Vertex<T, n, Args...>& vtx) {
    return vtx + v;
}

template<typename S, typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator-(const Vertex<T, n, Args...>& vtx, const Vector<S, n>& v) {
    return std::apply([&](auto... args) { return MakeVertex<T, n, Args...>((vtx.x - v).x, args...);}, vtx.args);
}
