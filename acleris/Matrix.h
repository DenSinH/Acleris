#pragma once

#include "Shapes.h"

#include <array>


template<typename T, int n, int m>
struct Matrix {
private:
    std::array<std::array<T, m>, n> e;

public:
    template<size_t... s>
    requires ((s <= n) && ...)
    Matrix(const T (&...list)[s]) : e{} {
        auto pos = &e[0][0];
        ((std::copy(list, list + s, pos), pos += n), ...);
    }

    std::array<T, m>& operator[](size_t index) {
        return e[index];
    }

    const std::array<T, m>& operator[](size_t index) const {
        return e[index];
    }

    template<typename S>
    Vector<S, n> operator*(const Vector<S, m>& v) const {
        std::array<S, n> x;
        for (int i = 0; i < n; i++) {
            std::common_type_t<T, S> value{};
            for (int j = 0; j < m; j++) {
                value += e[i][j] * v[j];
            }
            x[i] = value;
        }
        return Vector<S, n>(std::move(x));
    }

    template<typename S, typename... Args>
    Vertex<S, n, Args...> operator*(const Vertex<S, m, Args...>& v) const {

        return std::apply([&](auto... args) { return MakeVertex<S, n, Args...>((*this * v.x).x, args...); }, v.args);
    }

    template<typename V0>
    Point<V0> operator*(const Point<V0>& p) const {
        return Point<V0>(*this * p.v0);
    }

    template<typename V0, typename V1>
    Line<V0, V1> operator*(const Line<V0, V1>& l) const {
        return Line<V0, V1>(*this * l.v0, *this * l.v1);
    }

    template<typename V0, typename V1, typename V2>
    Triangle<V0, V1, V2> operator*(const Triangle<V0, V1, V2>& t) const {
        return Triangle<V0, V1, V2>(*this * t.v0, *this * t.v1, *this * t.v2);
    }
};