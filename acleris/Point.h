#pragma once

#include "util/Func.h"
#include "util/Tuple.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"


template<typename V0>
struct Point {
    const V0 v0;

    Point(const V0& v0) : v0(v0) { }

private:
    struct FragmentImpl {
        const V0& v0;
        const Color color;

        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);

            if (acleris.InBounds(v0)) {
                acleris.screen(acleris.width * v0.x[0], acleris.height * v0.x[1]) = color.ToRGBA8();
            }
        }
    };

public:
    auto Color(Color color) {
        return FragmentImpl{v0, color};
    }
};

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const Point<V0>& p, const Vector<T, n>& v) {
    return Point<V0>{p.v0 + v};
}

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const Vector<T, n>& v, const Point<V0>& p) {
    return p + v;
}

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator-(const Point<V0>& p, const Vector<T, n>& v) {
    return Point<V0>{p.v0 - v};
}