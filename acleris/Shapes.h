#pragma once


#include "Vertex.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "VMath/Matrix.h"


template< size_t n, typename... Args>
Vertex<n, Args...> operator*(const vmath::Matrix<float, n, n>& mat, const Vertex<n, Args...>& v) {
    return std::apply([&](auto... args) { return Vertex<n, Args...>(mat * v.x, args...); }, v.args);
}

template<size_t n, typename V0>
Point<V0> operator*(const vmath::Matrix<float, n, n>& mat, const Point<V0>& p) {
    return Point<V0>(mat * p.v0);
}

template<size_t n, typename V0, typename V1>
Line<V0, V1> operator*(const vmath::Matrix<float, n, n>& mat, const Line<V0, V1>& l) {
    return Line<V0, V1>(mat * l.v0, mat * l.v1);
}

template<size_t n, typename V0, typename V1, typename V2>
Triangle<V0, V1, V2> operator*(const vmath::Matrix<float, n, n>& mat, const Triangle<V0, V1, V2>& t) {
    return Triangle<V0, V1, V2>(mat * t.vert0, mat * t.vert1, mat * t.vert2);
}