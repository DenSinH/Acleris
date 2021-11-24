#pragma once

#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Vector.h"
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
            static constexpr size_t dim = V0::dim;

            auto screen_dim = v4{acleris.width, acleris.height, 1, 1};
            v4 _v0 = (v4{1, 1, 0, 0} + util::Project(v0.Extend4())) * screen_dim * v4{0.5, 0.5, 1, 1};

            const vmath::Vector<std::uint32_t, 4> screen_coords = (_v0 * screen_dim).convert<std::uint32_t>();

            if (acleris.InBounds(screen_coords)) {
                acleris.screen(screen_coords.get<0>(), screen_coords.get<1>()) = MakeRGBA8(color);
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