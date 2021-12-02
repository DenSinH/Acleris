#pragma once

#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Vector.h"
#include "util/Algorithm.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"
#include "DrawList.h"


template<typename V0>
struct Point {
    const V0 vert0;

    Point(const V0& vert0) : vert0(vert0) { }

private:
    class FragmentImpl final : public FragmentImplBase {
        const V0 vert0;
        const Color color;
    public:
        FragmentImpl(V0 vert0, Color color) :
                vert0(std::move(vert0)), color(std::move(color)) {

        }

        ~FragmentImpl() = default;
    private:

        friend struct DrawList;

        void DrawImpl(Acleris& acleris) final {
            static constexpr size_t dim = V0::dim;

            v4 _v0 = acleris.DeviceCoordinates(vert0);
            if (_v0.get<3>() < 0) return;

            float depth = _v0.get<2>();
            const vmath::Vector<std::uint32_t, 4> screen_coords = _v0.convert<std::uint32_t>();
            const int x = screen_coords.get<0>();
            const int y = screen_coords.get<1>();

            if (acleris.InBounds(screen_coords)) {
                if (acleris.CmpExchangeZ(depth, x, int(y))) {
                    acleris.screen(screen_coords.get<0>(), screen_coords.get<1>()) = RGBA8(color);
                }
            }
        }
    };

public:
    auto Color(Color color) {
        return std::make_unique<FragmentImpl>(vert0, color);
    }

    auto Color(std::uint32_t color) {
        return std::make_unique<FragmentImpl>(
                vert0,
                vmath::Vector<std::uint32_t, 1>(color).reinterpret<uint8_t>().convert<float>() * (1 / 255.0f)
        );
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