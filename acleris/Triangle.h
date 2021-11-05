#pragma once

#include "util/Point.h"
#include "Acleris.h"
#include "Vertex.h"


template<typename V0, typename V1, typename V2>
struct Triangle {
    const V0& v0;
    const V1& v1;
    const V2& v2;

    Triangle(const V0& v0, const V1& v1, const V2& v2) : v0(v0), v1(v1), v2(v2) { }

private:
    template<bool require_interp = true, typename F = std::uint32_t (*)(), typename T = std::common_type_t<typename V0::type, typename V1::type>>
    struct FragmentImpl {
        const V0& v0;
        const V1& v1;
        const V2& v2;
        const F& func;
    private:
        auto Interp(T x, T y) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V0::no_args == V1::no_args);

            if constexpr(require_interp) {
                static_assert(V0::dim == 2);
                static_assert(V1::dim == 2);
                static_assert(V2::dim == 2);
                static_assert(V0::no_args == V1::no_args);
                static_assert(V0::no_args == V2::no_args);

                auto l = BarycentricInterp(x, y, v0, v1, v2);
                const T l0 = l.first;
                const T l1 = l.second;
                const T l2 = 1 - l0 - l1;

                const auto op = [&](const auto& x, const auto& y, const auto& z) {
                    return l0 * x + l1 * y + l2 * z;
                };

                auto args = std::apply([&](const auto&... x){
                    return std::apply([&](const auto&... y){
                        return std::apply([&](const auto&... z){
                            return std::make_tuple(op(x, y, z)...);
                        }, v2.args);
                    }, v1.args);
                }, v0.args);

                return std::apply(func, args);
            }
            return std::apply(func, v0.args);
        }
    public:
        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V2::dim == 2);
            static_assert(V0::no_args == V1::no_args);
            static_assert(V0::no_args == V2::no_args);

            util::Point<T, 2> _v0 = {v0.x[0] * acleris.width, v0.x[1] * acleris.height};
            util::Point<T, 2> _v1 = {v1.x[0] * acleris.width, v1.x[1] * acleris.height};
            util::Point<T, 2> _v2 = {v2.x[0] * acleris.width, v2.x[1] * acleris.height};

            // sort by y coordinate
            if (_v1.x[1] < _v0.x[1]) {
                std::swap(_v0, _v1);
            }
            if (_v2.x[1] < _v0.x[1]) {
                std::swap(_v0, _v2);
            }
            if (_v2.x[1] < _v1.x[1]) {
                std::swap(_v1, _v2);
            }
            // first: go from top point to middle point
            T x1 = _v0.x[0], x2 = _v0.x[0];  // x1 will end up at v1 and x2 at v2
            T dx2 = T(_v2.x[0] - _v0.x[0]) / T(_v2.x[1] - _v0.x[1]);
            T dx1 = T(_v1.x[0] - _v0.x[0]) / T(_v1.x[1] - _v0.x[1]);

            T y = _v0.x[1];
            if (y < 0) {
                x1 += -y * dx1;
                x2 += -y * dx2;
                y = 0;
            }

            // draw part from v0 down to v1 (along v0 -- v1 and v0 -- v2)
            const int ymax = std::min<int>(acleris.height, _v1.x[1]);
            while (y < ymax) {
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                for (int x = std::max<int>(0, std::min(x1, x2)); x < xmax; x++) {
                    acleris.screen(x, y) = Interp(x / T(acleris.width), y / T(acleris.height));
                }
                x1 += dx1;
                x2 += dx2;
                y++;
            }

            // draw part from v1 down to v2 (along v0 -- v2 and v1 -- v2)
            const int ymax_ = std::min<int>(acleris.height, std::max(_v1.x[1], _v2.x[1]));
            x1 = _v1.x[0];  // just to be sure (should already be approx. the right value)
            dx1 = T(_v2.x[0] - _v1.x[0]) / T(_v2.x[1] - _v1.x[1]);

            while (y < ymax_) {
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                for (int x = std::max<int>(0, std::min(x1, x2)); x < xmax; x++) {
                    acleris.screen(x, y) = Interp(x / T(acleris.width), y / T(acleris.height));
                }
                x1 += dx1;
                x2 += dx2;
                y++;
            }
        }
    };
public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<true, F>{v0, v1, v2, func};
    }

    auto Color(std::uint32_t color) {
        const auto func = [&]{ return color; };
        return FragmentImpl<false, decltype(func)>{v0, v1, v2, func};
    }
};