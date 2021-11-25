#pragma once

#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Algorithm.h"
#include "util/Vector.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"


template<typename V0, typename V1, typename V2>
requires (V0::dim == V1::dim) && (V0::dim == V2::dim)
struct Triangle {
    const V0 vert0;
    const V1 vert1;
    const V2 vert2;

    Triangle(const V0& vert0, const V1& vert1, const V2& vert2) : vert0(vert0), vert1(vert1), vert2(vert2) { }

private:
    template<bool require_interp = true, typename F = Color (*)()>
    struct FragmentImpl {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0& vert0;
        const V1& vert1;
        const V2& vert2;
        const F func;

    private:
        auto Interp(float x, float y, const std::pair<float, float>& l) {
            static constexpr size_t dim = V0::dim;

            if constexpr(require_interp) {

                const float l0 = l.first;
                const float l1 = l.second;
                const float l2 = 1 - l0 - l1;

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
                        }, util::slice_tuple<min_size>(vert2.args));
                    }, util::slice_tuple<min_size>(vert1.args));
                }, util::slice_tuple<min_size>(vert0.args));

                return std::apply(func, args);
            }
            else {
                const auto args = util::slice_tuple<std::tuple_size_v<util::func_args_t<F>>>(vert0.args);
                return std::apply(func, args);
            }
        }
    public:
        void Draw(Acleris& acleris) {
            static constexpr size_t dim = V0::dim;

            // convert to device coordinates
            v4 _v0 = acleris.DeviceCoordinates(vert0);
            v4 _v1 = acleris.DeviceCoordinates(vert1);
            v4 _v2 = acleris.DeviceCoordinates(vert2);

            std::array<int, 3> _idx{0, 1, 2};

            // sort by y coordinate
            if (_v1.template get<1>() < _v0.get<1>()) {
                std::swap(_v0, _v1);
                std::swap(_idx[0], _idx[1]);
            }
            if (_v2.template get<1>() < _v0.get<1>()) {
                std::swap(_v0, _v2);
                std::swap(_idx[0], _idx[2]);
            }
            if (_v2.template get<1>() < _v1.get<1>()) {
                std::swap(_v1, _v2);
                std::swap(_idx[1], _idx[2]);
            }

            std::array<int, 3> idx{};
            for (int i = 0; i < 3; i++) {
                idx[_idx[i]] = i;
            }

            const v3 depth_inverse = {_v0.get<3>(), _v1.get<3>(), _v2.get<3>()};
            const v3 depth         = {_v0.get<2>(), _v1.get<2>(), _v2.get<2>()};

            // first: go from top point to middle point
            float x1 = _v0.get<0>(), x2 = _v0.get<0>();  // x1 will end up at v1 and x2 at v2
            const auto diff20 = _v2 - _v0;
            const auto diff10 = _v1 - _v0;
            float dx2 = diff20.get<0>() / diff20.get<1>();
            float dx1 = diff10.get<0>() / diff10.get<1>();

            float y = _v0.get<1>();
            if (y < 0) {
                x1 += -y * dx1;
                x2 += -y * dx2;
                y = 0;
            }

            // draw part from v0 down to v1 (along v0 -- v1 and v0 -- v2)
            const int ymax = std::min<int>(acleris.height, _v1.get<1>());

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
            const v2 dl01 = v2{-1, 1} * (1.0 / diff10.get<1>());
            const v2 dl02 = v2{-1, 0} * (1.0 / diff20.get<1>());

            // starting line might not be v0.y
            v2 l01 = v2{1, 0} + dl01 * (y - _v0.get<1>());
            v2 l02 = v2{1, 0} + dl02 * (y - _v0.get<1>());

            while (y < ymax) {
                // x bounds
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                const int xmin = std::max<int>(0, std::min(x1, x2));
                v2 l, dl;

                dl = (l02 - l01) * (1 / (x2 - x1));
                if (x1 < x2) {
                    // find starting l and dl for every line
                    l = l01;

                    // correct for the fact that the triangle might start off-screen
                    l  += (xmin - x1) * dl;
                }
                else {
                    // same here, except the bounds are flipped
                    l = l02;
                    l  += (xmin - x2) * dl;
                }

                for (int x = xmin; x < xmax; x++) {
                    v3 full_l = l.extend<3>() + v3{0, 0, 1 - l.sum()};
                    float _depth = full_l.dot(depth);

                    if (acleris.CmpExchangeZ(_depth, x, int(y))) {
                        std::uint32_t color;
                        v3 perspective = (full_l * depth_inverse) * (1 / full_l.dot(depth_inverse));

                        const std::array<float, 4> l_ = perspective.data();
                        if constexpr(require_interp) {
                            color = RGBA8(Interp(
                                    x / float(acleris.width), y / float(acleris.height),
                                    std::make_pair(l_[idx[0]], l_[idx[1]])
                            ));
                        }
                        else {
                            color = RGBA8(Interp(x / float(acleris.width), y / float(acleris.height), {}));
                        }

                        acleris.screen(x, int(y)) = color;
                    }
                    l  += dl;
                }
                x1 += dx1;
                x2 += dx2;
                y++;

                l01 += dl01;
                l02 += dl02;
            }

            // draw part from v1 down to v2 (along v0 -- v2 and v1 -- v2)
            const int ymax_ = std::min<int>(acleris.height, std::max(_v1.get<1>(), _v2.get<1>()));
            x1 = _v1.get<0>();  // just to be sure (should already be approx. the right value)
            const auto diff21 = _v2 - _v1;
            dx1 = diff21.get<0>() / diff21.get<1>();

            v2 l12 = v2{0, 1};
            const v2 dl12 = v2{0, -1} * (1 / diff21.get<1>());

            while (y < ymax_) {
                const int xmax = std::min<int>(acleris.width, std::max(x1, x2) + 1);
                const int xmin = std::max<int>(0, std::min(x1, x2));
                v2 l, dl;

                dl = (l02 - l12) * (1 / (x2 - x1));
                if (x1 < x2) {
                    l = l12;
                    l += (xmin - x1) * dl;
                }
                else {
                    l = l02;
                    l += (xmin - x2) * dl;
                }

                for (int x = xmin; x < xmax; x++) {
                    v3 full_l = l.extend<3>() + v3{0, 0, 1 - l.sum()};
                    float _depth = full_l.dot(depth);

                    if (acleris.CmpExchangeZ(_depth, x, int(y))) {
                        std::uint32_t color;
                        v3 perspective = (full_l * depth_inverse) * (1 / full_l.dot(depth_inverse));

                        const std::array<float, 4> l_ = perspective.data();
                        if constexpr(require_interp) {
                            color = RGBA8(Interp(
                                    x / float(acleris.width), y / float(acleris.height),
                                    std::make_pair(l_[idx[0]], l_[idx[1]])
                            ));
                        }
                        else {
                            color = RGBA8(Interp(x / float(acleris.width), y / float(acleris.height), {}));
                        }
                        acleris.screen(x, int(y)) = color;
                    }
                    l  += dl;
                }
                x1 += dx1;
                x2 += dx2;
                y++;

                if constexpr(require_interp) {
                    l12 += dl12;
                    l02 += dl02;
                }
            }
        }
    };
public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<true, F>{vert0, vert1, vert2, func};
    }

    auto Color(Color color) {
        const auto func = [&]{ return color; };
        return FragmentImpl<false, decltype(func)>{vert0, vert1, vert2, func};
    }
};


template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator+(const Triangle<V0, V1, V2>& t, const vmath::Vector<T, n>& v) {
    return Triangle<V0, V1, V2>{t.vert0 + v, t.vert1 + v, t.vert2 + v};
}

template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator+(const vmath::Vector<T, n>& v, const Triangle<V0, V1, V2>& t) {
    return t + v;
}

template<typename V0, typename V1, typename V2, typename T, size_t n>
requires (V0::dim == n) && (V1::dim == n) && (V2::dim == n)
Triangle<V0, V1, V2> operator-(const Triangle<V0, V1, V2>& t, const vmath::Vector<T, n>& v) {
    return Triangle<V0, V1, V2>{t.vert0 - v, t.vert1 - v, t.vert2 - v};
}
