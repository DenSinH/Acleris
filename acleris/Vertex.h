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


//template<typename T, typename V0, typename V1, typename V2>
//std::pair<T, T> BarycentricInterp(T x, T y, const V0& v0, const V1& v1, const V2& v2) {
//    static_assert(V0::dim == 2);
//    static_assert(V1::dim == 2);
//    static_assert(V2::dim == 2);
//
//    // barycentric coordinates: https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Edge_approach
//    const T det = T(v1.x[1] - v2.x[1]) * T(v0.x[0] - v2.x[0]) + T(v2.x[0] - v1.x[0]) * T(v0.x[1] - v2.x[1]);
//    const T l0 = T(v1.x[1] - v2.x[1]) * T(x - v2.x[0]) + T(v2.x[0] - v1.x[0]) * T(y - v2.x[1]);
//    const T l1 = T(v2.x[1] - v0.x[1]) * T(x - v2.x[0]) + T(v0.x[0] - v2.x[0]) * T(y - v2.x[1]);
//    return std::make_pair(l0 / det, l1 / det);
//}

template<typename T, int n, typename... Args>
Vertex<T, n, Args...> MakeVertex(std::array<T, n> x, Args... args) {
    return Vertex<T, n, Args...>(std::move(x), args...);
}