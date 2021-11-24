#pragma once


using v4 = vmath::Vector<float, 4>;
using v3 = vmath::Vector<float, 3>;
using v2 = vmath::Vector<float, 2>;

namespace util {

static v4 Project(const v4& v) {
    // v.xyz /= v.w
    // v.w = 1 / v.w
    auto rv = v4{1 / v.get<3>()};
    return v.mask_mul(rv, vmath::Mask(0, 1, 2), rv);
}

}