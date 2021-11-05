#pragma once

#include "util/Point.h"
#include "Acleris.h"
#include "Vertex.h"


template<typename V0, typename V1>
struct Line {
    const V0& v0;
    const V1& v1;

    Line(const V0& v0, const V1& v1) : v0(v0), v1(v1) { }


private:
    template<bool require_interp = true, typename F = std::uint32_t (*)(), typename T = std::common_type_t<typename V0::type, typename V1::type>>
    struct FragmentImpl {
        const V0& v0;
        const V1& v1;
        const F& func;
    private:
        auto Interp(T x, T y) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V0::no_args == V1::no_args);

            if constexpr(require_interp) {
                const T len = (v1.x[0] - v0.x[0]) * (v1.x[0] - v0.x[0]) + (v1.x[1] - v0.x[1]) * (v1.x[1] - v0.x[1]);
                const T l0 = ((x - v0.x[0]) * (x - v0.x[0]) + (y - v0.x[1]) * (y - v0.x[1])) / len;
                const T l1 = 1 - l0;

                const auto op = [&](const auto& x, const auto& y) {
                    return l0 * x + l1 * y;
                };

                auto args = std::apply([&](const auto&... x){
                    return std::apply([&](const auto&... y){
                        return std::make_tuple(op(x, y)...);
                    }, v1.args);
                }, v0.args);

                return std::apply(func, args);
            }
            return std::apply(func, v0.args);
        }

        template<bool xmajor>
        void Draw(Acleris& acleris) {
            // for y-major lines, the algorithm is precisely the same, except x and y are swapped

            util::Point<T, 2> _v0 = {v0.x[0] * acleris.width, v0.x[1] * acleris.height};
            util::Point<T, 2> _v1 = {v1.x[0] * acleris.width, v1.x[1] * acleris.height};
            if constexpr(!xmajor) {
                std::swap(_v0.x[0], _v0.x[1]);
                std::swap(_v1.x[0], _v1.x[1]);
            }

            // from left to right
            if (_v1.x[0] < _v0.x[0]) {
                std::swap(_v0, _v1);
            }

            // initial coordinates
            T x = _v0.x[0], y = _v0.x[1];
            T dy = float(_v1.x[1] - _v0.x[1]) / float(_v1.x[0] - _v0.x[0]);

            const auto xbound = (xmajor ? acleris.width : acleris.height);
            if (x < 0) {
                // clip to screen boundary
                y += -x * dy;
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

                // line ended before reaching screen
                if (x > _v1.x[0]) [[unlikely]] return;
                y = 0;
            }
            else if (y > ybound) {
                // entire line is off screen
                if (dy > 0) [[unlikely]] return;
                x += (y - ybound) / dy;

                // line ended before reaching screen
                if (x > _v1.x[0]) [[unlikely]] return;
                y = ybound;
            }

            // need to flip coordinates for x/y major
            if constexpr(xmajor) {
                for (int x_ = int(x); x_ < _v1.x[0] && acleris.InBounds(x_, int(y)); x_++) {
                    acleris.screen(int(x_), int(y)) = Interp(x_ / T(acleris.width), y / T(acleris.height));
                    y += dy;
                }
            }
            else {
                for (int x_ = int(x); x_ < _v1.x[0] && acleris.InBounds(int(y), x_); x_++) {
                    acleris.screen(int(y), int(x_)) = Interp(y / T(acleris.width), x_ / T(acleris.height));
                    y += dy;
                }
            }
        }

    public:
        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V0::no_args == V1::no_args);

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

    auto Color(std::uint32_t color) {
        const auto func = [=]{ return color; };
        return FragmentImpl<false, decltype(func)>{v0, v1, func};
    }
};