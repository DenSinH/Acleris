#pragma once

#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Algorithm.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"


template<typename V0, typename V1, typename V2>
struct Triangle {
    const V0 v0;
    const V1 v1;
    const V2 v2;

    Triangle(const V0& v0, const V1& v1, const V2& v2) : v0(v0), v1(v1), v2(v2) { }

private:
    template<bool require_interp = true, typename F = Color (*)(), typename T = std::common_type_t<typename V0::type, typename V1::type>>
    struct FragmentImpl {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0& v0;
        const V1& v1;
        const V2& v2;
        const F func;

    private:
        auto Interp(T x, T y, const std::pair<T, T>& l) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V2::dim == 2);

            if constexpr(require_interp) {

                const T l0 = l.first;
                const T l1 = l.second;
                const T l2 = 1 - l0 - l1;

                const auto op = [&](const auto& x, const auto& y, const auto& z) {
                    return l0 * x + l1 * y + l2 * z;
                };

                // we might have vertices with too many extra arguments / different sizes
                constexpr auto min_size = util::min(
                        V0::no_args, V1::no_args, V2::no_args, std::tuple_size_v<util::func_args_t<F>>
                );

                const auto args = std::apply([&](const auto&... x){
                    return std::apply([&](const auto&... y){
                        return std::apply([&](const auto&... z){
                            return std::make_tuple(op(x, y, z)...);
                        }, util::slice_tuple<min_size>(v2.args));
                    }, util::slice_tuple<min_size>(v1.args));
                }, util::slice_tuple<min_size>(v0.args));

                return std::apply(func, args);
            }
            else {
                const auto args = util::slice_tuple<std::tuple_size_v<util::func_args_t<F>>>(v0.args);
                return std::apply(func, args);
            }
        }
    public:
        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);
            static_assert(V1::dim == 2);
            static_assert(V2::dim == 2);

            Vector<T, 2> _v0 = {v0.x[0] * acleris.width, v0.x[1] * acleris.height};
            Vector<T, 2> _v1 = {v1.x[0] * acleris.width, v1.x[1] * acleris.height};
            Vector<T, 2> _v2 = {v2.x[0] * acleris.width, v2.x[1] * acleris.height};

            std::array<int, 3> _idx{0, 1, 2};

            // sort by y coordinate
            if (_v1.x[1] < _v0.x[1]) {
                std::swap(_v0, _v1);
                std::swap(_idx[0], _idx[1]);
            }
            if (_v2.x[1] < _v0.x[1]) {
                std::swap(_v0, _v2);
                std::swap(_idx[0], _idx[2]);
            }
            if (_v2.x[1] < _v1.x[1]) {
                std::swap(_v1, _v2);
                std::swap(_idx[1], _idx[2]);
            }

            std::array<int, 3> idx{};
            for (int i = 0; i < 3; i++) {
                idx[_idx[i]] = i;
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

            /* Faster Barycentric interpolation:
             * Lerp between the edges, then between the bounds
             * We fill the triangle in 2 loops:
             * First we go down from v0 to v1, then down from v1 to v2.
             * For the first part, l2 (lambda 2, third barycentric coodrinate) is always 0
             * For the second part, l0 is always 0.
             *
             *          /| v0
             *         / | (100)
             *        /  |
             *    v1 /___|
             * (010) \   |
             *        \  |
             *         \ |
             *          \| v2
             *             (001)
             * */

            // difference per line
            const T dl01 = 1.0 / T(_v1.x[1] - _v0.x[1]);
            const T dl02 = 1.0 / T(_v2.x[1] - _v0.x[1]);

            // starting line might not be v0.y
            std::pair<T, T> l01 = std::make_pair(1 - dl01 * (y - _v0.x[1]), dl01 * (y - _v0.x[1]));
            std::pair<T, T> l02 = std::make_pair(1 - dl02 * (y - _v0.x[1]), 0);

            while (y < ymax) {
                // x bounds
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                const int xmin = std::max<int>(0, std::min(x1, x2));
                std::pair<T, T> l, dl;

                if constexpr(require_interp) {
                    if (x1 < x2) {
                        // find starting l and dl for every line
                        l = std::make_pair(l01.first, l01.second);
                        dl = std::make_pair((l02.first - l01.first) / T(x2 - x1), -l01.second / T(x2 - x1));

                        // correct for the fact that the triangle might start off-screen
                        l.first  += (xmin - x1) * dl.first;
                        l.second += (xmin - x1) * dl.second;
                    }
                    else {
                        // same here, except the bounds are flipped
                        l = std::make_pair(l02.first, 0);
                        dl = std::make_pair((l01.first - l02.first) / T(x1 - x2), l01.second / T(x1 - x2));
                        l.first  += (xmin - x2) * dl.first;
                        l.second += (xmin - x2) * dl.second;
                    }
                }

                for (int x = xmin; x < xmax; x++) {
                    if constexpr(require_interp) {
                        const std::array<T, 3> l_ = {l.first, l.second, 1 - l.first - l.second};
                        acleris.screen(x, int(y)) = Interp(
                                x / T(acleris.width), y / T(acleris.height),
                                std::make_pair(l_[idx[0]], l_[idx[1]])
                        ).ToRGBA8();
                        l.first  += dl.first;
                        l.second += dl.second;
                    }
                    else {
                        acleris.screen(x, int(y)) = Interp(x / T(acleris.width), y / T(acleris.height), {}).ToRGBA8();
                    }
                }
                x1 += dx1;
                x2 += dx2;
                y++;

                if constexpr(require_interp) {
                    l01.first  -= dl01;
                    l01.second += dl01;
                    l02.first  -= dl02;
                }
            }

            // draw part from v1 down to v2 (along v0 -- v2 and v1 -- v2)
            const int ymax_ = std::min<int>(acleris.height, std::max(_v1.x[1], _v2.x[1]));
            x1 = _v1.x[0];  // just to be sure (should already be approx. the right value)
            dx1 = T(_v2.x[0] - _v1.x[0]) / T(_v2.x[1] - _v1.x[1]);

            std::pair<T, T> l12 = std::make_pair(1, 0);
            const T dl12 = 1 / T(_v2.x[1] - _v1.x[1]);

            while (y < ymax_) {
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                const int xmin = std::max<int>(0, std::min(x1, x2));
                std::pair<T, T> l, dl;

                if constexpr(require_interp) {
                    if (x1 < x2) {
                        l = std::make_pair(0, l12.first);
                        dl = std::make_pair(l02.first / T(x2 - x1), -l12.first / T(x2 - x1));
                        l.first  += (xmin - x1) * dl.first;
                        l.second += (xmin - x1) * dl.second;
                    }
                    else {
                        l = std::make_pair(l02.first, 0);
                        dl = std::make_pair(-l02.first / T(x1 - x2), l12.first / T(x1 - x2));
                        l.first  += (xmin - x2) * dl.first;
                        l.second += (xmin - x2) * dl.second;
                    }
                }

                for (int x = xmin; x < xmax; x++) {
                    if constexpr(require_interp) {
                        const std::array<T, 3> l_ = {l.first, l.second, 1 - l.first - l.second};
                        acleris.screen(x, int(y)) = Interp(
                                x / T(acleris.width), y / T(acleris.height),
                                std::make_pair(l_[idx[0]], l_[idx[1]])
                        ).ToRGBA8();
                        l.first  += dl.first;
                        l.second += dl.second;
                    }
                    else {
                        acleris.screen(x, int(y)) = Interp(x / T(acleris.width), y / T(acleris.height), {}).ToRGBA8();
                    }
                }
                x1 += dx1;
                x2 += dx2;
                y++;

                if constexpr(require_interp) {
                    l12.first -= dl12;
                    l02.first -= dl02;
                }
            }
        }
    };
public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<true, F>{v0, v1, v2, func};
    }

    auto Color(Color color) {
        const auto func = [&]{ return color; };
        return FragmentImpl<false, decltype(func)>{v0, v1, v2, func};
    }
};


template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator+(const Triangle<V0, V1, V2>& t, const Vector<T, n>& v) {
    return Triangle<V0, V1, V2>{t.v0 + v, t.v1 + v, t.v2 + v};
}

template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator+(const Vector<T, n>& v, const Triangle<V0, V1, V2>& t) {
    return t + v;
}

template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator-(const Triangle<V0, V1, V2>& t, const Vector<T, n>& v) {
    return Triangle<V0, V1, V2>{t.v0 - v, t.v1 - v, t.v2 - v};
}
