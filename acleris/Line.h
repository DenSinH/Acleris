#pragma once

#include "util/Algorithm.h"
#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Vector.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"

#include <cmath>


template<typename V0, typename V1>
requires (V0::dim == V1::dim)
struct Line {
    const V0 v0;
    const V1 v1;

    Line(const V0& v0, const V1& v1) : v0(v0), v1(v1) { }

private:
    template<bool require_interp = true, typename F = Color (*)()>
    struct FragmentImpl {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0& v0;
        const V1& v1;
        const F func;
    private:
        auto Interp(float x, float y, const float l0) {
            if constexpr(require_interp) {
                const float l1 = 1 - l0;

                const auto op = [&](const auto& x, const auto& y) {
                    return l0 * x + l1 * y;
                };

                // we might have vertices with too many extra arguments / different sizes
                constexpr auto min_size = util::min(V0::no_args, V1::no_args, std::tuple_size_v<util::func_args_t<F>>);

                auto args = std::apply([&](const auto&... x){
                    return std::apply([&](const auto&... y){
                        return std::make_tuple(op(x, y)...);
                    }, util::slice_tuple<min_size>(v1.args));
                }, util::slice_tuple<min_size>(v0.args));

                return std::apply(func, args);
            }
            else {
                auto args = util::slice_tuple<util::tuple_size(std::make_tuple<util::func_args_t<F>>())>(v0.args);
                return std::apply(func, args);
            }
        }

        template<bool xmajor>
        void Draw(Acleris& acleris) {
            // for y-major lines, the algorithm is precisely the same, except x and y are swapped
            static constexpr size_t dim = V0::dim;

            auto screen_dim = v4{acleris.width, acleris.height, 1, 1};
            v4 _v0 = (v4{1, 1, 0, 0} + util::Project(v0.Extend4())) * screen_dim * v4{0.5, 0.5, 1, 1};
            v4 _v1 = (v4{1, 1, 0, 0} + util::Project(v1.Extend4())) * screen_dim * v4{0.5, 0.5, 1, 1};
            // todo: perspective correct interpolation
            _v0 = acleris.view * _v0;
            _v1 = acleris.view * _v1;

            float l0 = 0;

            if constexpr(!xmajor) {
                // todo: do this better
                _v0 = {_v0.get<1>(), _v0.get<0>(), _v0.get<2>(), _v0.get<3>()};
                _v1 = {_v1.get<1>(), _v1.get<0>(), _v1.get<2>(), _v1.get<3>()};
            }

            // from left to right
            if (_v1.get<0>() < _v0.get<0>()) {
                std::swap(_v0, _v1);
                l0 = 1;
            }

            // initial coordinates
            float x = _v0.get<0>(), y = _v0.get<1>();
            auto diff = _v1 - _v0;
            float dy = diff.get<1>() / diff.get<0>();

            const auto xbound = (xmajor ? acleris.width : acleris.height);
            const auto square = diff * diff;
            const float len = square.get<0>() + square.get<1>();
            const float dl = (l0 ? -1 : 1) * std::sqrt((1 + dy * dy) / len);
            if (x < 0) {
                // clip to screen boundary
                y += -x * dy;
                l0 += -x * dy * dl;
                x = 0;
                if (_v1.get<0>() < 0) [[unlikely]] return;  // entire line is off screen
            }
            else if (x > xbound) {
                return;
            }

            const auto ybound = (xmajor ? acleris.height : acleris.width);
            if (y < 0) {
                // entire line is off screen
                if (dy < 0) [[unlikely]] return;
                x += -y / dy;
                l0 += (-y / dy) * dl;

                // line ended before reaching screen
                if (x > _v1.get<0>()) [[unlikely]] return;
                y = 0;
            }
            else if (y > ybound) {
                // entire line is off screen
                if (dy > 0) [[unlikely]] return;
                x += (y - ybound) / dy;
                l0 += ((y - ybound) / dy) * dl;

                // line ended before reaching screen
                if (x > _v1.get<0>()) [[unlikely]] return;
                y = ybound;
            }

            // need to flip coordinates for x/y major
            if constexpr(xmajor) {
                for (int x_ = int(x); x_ < _v1.get<0>() && acleris.InBounds(x_, int(y)); x_++) {
                    acleris.screen(int(x_), int(y)) = MakeRGBA8(Interp(x_ / float(acleris.width), y / float(acleris.height), l0));
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
            else {
                for (int x_ = int(x); x_ < _v1.get<0>() && acleris.InBounds(int(y), x_); x_++) {
                    acleris.screen(int(y), int(x_)) = MakeRGBA8(Interp(y / float(acleris.width), x_ / float(acleris.height), l0));
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
        }

    public:
        void Draw(Acleris& acleris) {
            // find out if the line is x-major or y-major
            // |y1 - y0| < |x1 - x0| <==> x-major (not steep)
            const auto diff = (v1.x - v0.x).abs();
            bool xmajor = diff.template get<0>() >= diff.template get<1>();
            if (xmajor) {
                Draw<true>(acleris);
            }
            else {
                Draw<false>(acleris);
            }
        }
    };

public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<true, F>{v0, v1, func};
    }

    auto Color(Color color) {
        const auto func = [=]{ return color; };
        return FragmentImpl<false, decltype(func)>{v0, v1, func};
    }
};

template<typename V0, typename V1, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator+(const Line<V0, V1>& l, const vmath::Vector<float, n>& v) {
    return Line<V0, V1>{l.v0 + v, l.v1 + v};
}

template<typename V0, typename V1, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator+(const vmath::Vector<float, n>& v, const Line<V0, V1>& l) {
    return l + v;
}

template<typename V0, typename V1, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator-(const Line<V0, V1>& l, const vmath::Vector<float, n>& v) {
    return Line<V0, V1>{l.v0 - v, l.v1 - v};
}