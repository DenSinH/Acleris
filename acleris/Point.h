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
    template<typename F>
    struct FragmentImpl {
        static_assert(std::is_same_v<util::func_return_t<F>, Color>);

        const V0& v0;
        const F& func;

        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);

            if (acleris.InBounds(v0)) {
                auto args = util::slice_tuple<std::tuple_size_v<util::func_args_t<F>>>(v0.args);
                acleris.screen(acleris.width * v0.x[0], acleris.height * v0.x[1]) = std::apply(func, args).ToRGBA8();
            }
        }
    };

public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<F>{v0, func};
    }

    auto Color(Color color) {
        auto func = [=]{ return color; };
        return Fragment<decltype(func)>(func);
    }
};

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const Point<V0>& p, const Vector<T, n>& v) {
    return Point<V0>{p.v0 + v};
}

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator+(const Vector<T, n>& v, const Point<V0>& p) {
    return p + v;
}

template<typename V0, typename T, size_t n>
requires (V0::dim == n)
Point<V0> operator-(const Point<V0>& p, const Vector<T, n>& v) {
    return Point<V0>{p.v0 - v};
}