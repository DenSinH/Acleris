#pragma once

#include <algorithm>


struct Color {
    float r, g, b, a;

    Color(float r, float g, float b, float a = 1.0) :
            r(r), g(g), b(b), a(a) {

    }

    template<typename T>
    Color operator*(const T& value) const {
        return Color{
            value * r,
            value * g,
            value * b,
            value * a,
        };
    }

    Color operator+(const Color& other) const {
        return Color{
            other.r + r,
            other.g + g,
            other.b + b,
            other.a + a,
        };
    }

    std::uint32_t ToRGBA8() {
        return (
                (std::uint32_t(0xff * Clamp(r)) << 24)
                | (std::uint32_t(0xff * Clamp(g)) << 16)
                | (std::uint32_t(0xff * Clamp(b)) << 8)
                | (std::uint32_t(0xff * Clamp(a)))
        );
    }

private:
    static constexpr float Clamp(float value) {
        return std::clamp(value, 0.0f, 1.0f);
    }
};


template<typename T>
Color operator*(const T& value, const Color& color) {
    return Color{
            value * color.r,
            value * color.g,
            value * color.b,
            value * color.a,
    };
}