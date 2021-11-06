#pragma once

#include "Vector.h"

#include <array>
#include <tuple>


template<typename T, int n, typename... Args>
struct Vertex {
    static constexpr int dim = n;
    static constexpr int no_args = sizeof...(Args);
    using type = T;

    Vector<T, n> x{};
    const std::tuple<Args...> args;

    Vertex(std::array<T, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }
};

template<typename T, int n, typename... Args>
Vertex<T, n, Args...> MakeVertex(std::array<T, n> x, Args... args) {
    return Vertex<T, n, Args...>(std::move(x), args...);
}