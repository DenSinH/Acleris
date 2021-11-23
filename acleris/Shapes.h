#pragma once


#include "Vertex.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "VMath/Matrix.h"


template<typename T, size_t n, typename... Args>
Vertex<T, n, Args...> operator*(const vmath::Matrix<T, n, n>& mat, const Vertex<T, n, Args...>& v) {
    return std::apply([&](auto... args) { return Vertex<T, n, Args...>(mat * v.x, args...); }, v.args);
}

template<typename T, size_t n, typename V0>
Point<V0> operator*(const vmath::Matrix<T, n, n>& mat, const Point<V0>& p) {
    return Point<V0>(mat * p.v0);
}

template<typename T, size_t n, typename V0, typename V1>
Line<V0, V1> operator*(const vmath::Matrix<T, n, n>& mat, const Line<V0, V1>& l) {
    return Line<V0, V1>(mat * l.v0, mat * l.v1);
}

template<typename T, size_t n, typename V0, typename V1, typename V2>
Triangle<V0, V1, V2> operator*(const vmath::Matrix<T, n, n>& mat, const Triangle<V0, V1, V2>& t) {
    return Triangle<V0, V1, V2>(mat * t.v0, mat * t.v1, mat * t.v2);
}