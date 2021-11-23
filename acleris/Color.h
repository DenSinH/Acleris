#pragma once

#include "VMath/Vector.h"

using Color = vmath::Vector<float, 4>;

Color MakeColor(float r, float g, float b, float a = 1.0) {
    return Color{a, b, g, r};
}

std::uint32_t MakeRGBA8(const Color& color) {
    return (color.clamp(0, 1) * 0xff).convert<std::uint8_t>().reinterpret<std::uint32_t>().get<0>();
}