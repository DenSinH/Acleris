#pragma once

#include "VMath/Vector.h"

using Color = vmath::Vector<float, 4>;

Color RGB(float r, float g, float b) {
    return Color{1.0, b, g, r};
}

Color RGBA(float r, float g, float b, float a) {
    return Color{a, b, g, r};
}

std::uint32_t RGBA8(const Color& color) {
    return (color.clamp(0, 1) * 0xff).convert<std::uint8_t>().reinterpret<std::uint32_t>().get<0>();
}