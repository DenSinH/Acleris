#pragma once

#include "VMath/Vector.h"
#include "VMath/Matrix.h"

using v4 = vmath::Vector<float, 4>;
using v3 = vmath::Vector<float, 3>;
using v2 = vmath::Vector<float, 2>;

using m4x4 = vmath::Matrix<float, 4, 4>;
using m3x3 = vmath::Matrix<float, 3, 3>;
using m2x2 = vmath::Matrix<float, 2, 2>;

namespace util {

static v4 Project(const v4& v) {
    // v.xyz /= v.w
    // v.w = 1 / v.w
    auto rv = v4{1 / v.get<3>()};
    return v.mask_mul(rv, vmath::Mask(0, 1, 2), rv);
}

}