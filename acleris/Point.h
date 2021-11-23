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

            const vmath::Vector<float, 2> screen_dim = {acleris.width, acleris.height};
            const vmath::Vector<std::uint32_t, 2> screen_coords = (v0.x * screen_dim).template convert<std::uint32_t>();

            if (acleris.InBounds(screen_coords)) {
                acleris.screen(screen_coords.template get<0>(), screen_coords.template get<1>()) = MakeRGBA8(color);
            }
        }
    };

public:
    auto Color(Color color) {
        return FragmentImpl{v0, color};
    }

    auto Color(std::uint32_t color) {
        return FragmentImpl{v0, vmath::Vector<std::uint32_t, 1>(color).reinterpret<uint8_t>().convert<float>() * (1 / 255.0f)};
    }
};

template<typename V0, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const Point<V0>& p, const vmath::Vector<float, n>& v) {
    return Point<V0>{p.v0 + v};
}

template<typename V0, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const vmath::Vector<float, n>& v, const Point<V0>& p) {
    return p + v;
}

template<typename V0, size_t n>
requires (V0::dim == n)
Point<V0> operator-(const Point<V0>& p, const vmath::Vector<float, n>& v) {
    return Point<V0>{p.v0 - v};
}