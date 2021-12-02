#pragma once

#include "util/Algorithm.h"
#include "util/Func.h"
#include "util/Tuple.h"
#include "util/Vector.h"
#include "Acleris.h"
#include "Vertex.h"
#include "Color.h"
#include "DrawList.h"

#include <cmath>


template<typename V0, typename V1>
requires (V0::dim == V1::dim)
struct Line {
    const V0 vert0;
    const V1 vert1;

    Line(const V0& vert0, const V1& vert1) : vert0(vert0), vert1(vert1) { }

private:
    template<bool require_interp = true, typename F = Color (*)()>
    class FragmentImpl : public FragmentImplBase {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0 vert0;
        const V1 vert1;
        const F func;
    public:
        FragmentImpl(V0 vert0, V1 vert1, F func) :
                vert0(std::move(vert0)), vert1(std::move(vert1)), func(std::move(func)) {

        }

        ~FragmentImpl() = default;
    private:

        friend struct DrawList;

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
                    }, util::slice_tuple<min_size>(vert1.args));
                }, util::slice_tuple<min_size>(vert0.args));

                return std::apply(func, args);
            }
            else {
                // auto args = util::slice_tuple<util::tuple_size(std::make_tuple<util::func_args_t<F>>())>(vert0.args);
                return func();
            }
        }

        template<bool xmajor>
        void DrawImpl(Acleris& acleris, v4 _v0, v4 _v1) {
            // for y-major lines, the algorithm is precisely the same, except x and y are swapped
            static constexpr size_t dim = V0::dim;

            if (_v0.get<3>() < 0) return;
            if (_v1.get<3>() < 0) return;

            auto clip0 = acleris.Clip(_v0);
            auto clip1 = acleris.Clip(_v1);
            if (clip0 && (clip0 == clip1)) {
                // discard primitive
                return;
            }

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
            float x = _v0.get<0>();
            float y = _v0.get<1>();
            const auto diff = _v1 - _v0;
            float dy = diff.get<1>() / diff.get<0>();

            const auto xbound = (xmajor ? acleris.width : acleris.height);
            const float dl = (l0 ? -1 : 1) * std::sqrt((1 + dy * dy) / diff.dot(diff));

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
                    const float depth = l0 * _v0.get<2>() + (1 - l0) * _v1.get<2>();

                    acleris.AccessRegion(x_, int(y), [&]{
                        if (acleris.CmpExchangeZ(depth, x_, y)) {
                            acleris.screen(int(x_), int(y)) = RGBA8(
                                    Interp(x_ / float(acleris.width), y / float(acleris.height), l0)
                            );
                        }
                    });
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
            else {
                for (int x_ = int(x); x_ < _v1.get<0>() && acleris.InBounds(int(y), x_); x_++) {
                    const float depth = l0 * _v0.get<2>() + (1 - l0) * _v1.get<2>();

                    acleris.AccessRegion(int(y), int(x_), [&]{
                        if (acleris.CmpExchangeZ(depth, y, x_)) {
                            acleris.screen(int(y), int(x_)) = RGBA8(
                                    Interp(y / float(acleris.width), x_ / float(acleris.height), l0)
                            );
                        }
                    });
                    y += dy;
                    if constexpr(require_interp) {
                        l0 += dl;
                    }
                }
            }
        }

        void DrawImpl(Acleris& acleris) final {
            // find out if the line is x-major or y-major
            // |y1 - y0| < |x1 - x0| <==> x-major (not steep)
            v4 _v0 = acleris.DeviceCoordinates(vert0);
            v4 _v1 = acleris.DeviceCoordinates(vert1);

            const auto diff = (_v1 - _v0).abs();
            bool xmajor = diff.template get<0>() >= diff.template get<1>();
            if (xmajor) {
                DrawImpl<true>(acleris, _v0, _v1);
            }
            else {
                DrawImpl<false>(acleris, _v0, _v1);
            }
        }
    };

public:
    template<typename F>
    auto Fragment(F func) {
        return std::make_unique<FragmentImpl<true, F>>(vert0, vert1, func);
    }

    auto Color(Color color) {
        const auto func = [=]{ return color; };
        return std::make_unique<FragmentImpl<false, decltype(func)>>(vert0, vert1, func);
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