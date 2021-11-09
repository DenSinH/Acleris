#pragma once

#include "util/Algorithm.h"
#include "util/Func.h"
#include "util/Tuple.h"
#include "Acleris.h"
#include "Vertex.h"

#include <cmath>


template<typename V0, typename V1>
struct Line {
    const V0 v0;
    const V1 v1;

    Line(const V0& v0, const V1& v1) : v0(v0), v1(v1) { }

private:
    template<bool require_interp = true, typename F = Color (*)(), typename T = std::common_type_t<typename V0::type, typename V1::type>>
    struct FragmentImpl {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0& v0;
        const V1& v1;
        const F func;
    private:
        auto Interp(T x, T y, const T& l0) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);

            if constexpr(require_interp) {
                const T l1 = 1 - l0;

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

            Vector<T, 2> _v0 = {v0.x[0] * acleris.width, v0.x[1] * acleris.height};
            Vector<T, 2> _v1 = {v1.x[0] * acleris.width, v1.x[1] * acleris.height};
            T l0 = 0;

            if constexpr(!xmajor) {
                std::swap(_v0.x[0], _v0.x[1]);
                std::swap(_v1.x[0], _v1.x[1]);
            }

            // from left to right
            if (_v1.x[0] < _v0.x[0]) {
                std::swap(_v0, _v1);
                l0 = 1;
            }

            // initial coordinates
            T x = _v0.x[0], y = _v0.x[1];
            T dy = float(_v1.x[1] - _v0.x[1]) / float(_v1.x[0] - _v0.x[0]);

            const auto xbound = (xmajor ? acleris.width : acleris.height);
            const T len = (_v1.x[0] - _v0.x[0]) * (_v1.x[0] - _v0.x[0]) + (_v1.x[1] - _v0.x[1]) * (_v1.x[1] - _v0.x[1]);
            const T dl = (l0 ? -1 : 1) * std::sqrt((1 + dy * dy) / len);
            if (x < 0) {
                // clip to screen boundary
                y += -x * dy;
                l0 += -x * dy * dl;
                x = 0;
                if (_v1.x[0] < 0) [[unlikely]] return;  // entire line is off screen
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
                if (x > _v1.x[0]) [[unlikely]] return;
                y = 0;
            }
            else if (y > ybound) {
                // entire line is off screen
                if (dy > 0) [[unlikely]] return;
                x += (y - ybound) / dy;
                l0 += ((y - ybound) / dy) * dl;

                // line ended before reaching screen
                if (x > _v1.x[0]) [[unlikely]] return;
                y = ybound;
            }

            // need to flip coordinates for x/y major
            if constexpr(xmajor) {
                for (int x_ = int(x); x_ < _v1.x[0] && acleris.InBounds(x_, int(y)); x_++) {
                    acleris.screen(int(x_), int(y)) = Interp(x_ / T(acleris.width), y / T(acleris.height), l0).ToRGBA8();
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
            else {
                for (int x_ = int(x); x_ < _v1.x[0] && acleris.InBounds(int(y), x_); x_++) {
                    acleris.screen(int(y), int(x_)) = Interp(y / T(acleris.width), x_ / T(acleris.height), l0).ToRGBA8();
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
        }

    public:
        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);

            // find out if the line is x-major or y-major
            // |y1 - y0| < |x1 - x0| <==> x-major (not steep)
            bool xmajor = std::abs(v1.x[0] - v0.x[0]) >= std::abs(v1.x[1] - v0.x[1]);
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

template<typename V0, typename V1, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator+(const Line<V0, V1>& l, const Vector<T, n>& v) {
    return Line<V0, V1>{l.v0 + v, l.v1 + v};
}

template<typename V0, typename V1, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator+(const Vector<T, n>& v, const Line<V0, V1>& l) {
    return l + v;
}

template<typename V0, typename V1, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n)
Line<V0, V1> operator-(const Line<V0, V1>& l, const Vector<T, n>& v) {
    return Line<V0, V1>{l.v0 - v, l.v1 - v};
}