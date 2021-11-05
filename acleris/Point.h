#pragma once

#include "Acleris.h"
#include "Vertex.h"


template<typename V0>
struct Point {
    const V0& v0;

    Point(const V0& v0) : v0(v0) { }

private:
    template<typename F>
    struct FragmentImpl {
        const V0& v0;
        const F& func;

        void Draw(Acleris& acleris) {
            static_assert(V0::dim == 2);

            if (acleris.InBounds(v0)) {
                acleris.screen(acleris.width * v0.x[0], acleris.height * v0.x[1]) = std::apply(func, v0.args);
            }
        }
    };

public:
    template<typename F>
    auto Fragment(F func) {
        return FragmentImpl<F>{v0, func};
    }

    auto Color(std::uint32_t color) {
        auto func = [=]{ return color; };
        return Fragment<decltype(func)>(func);
    }
};
