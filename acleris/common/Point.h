#pragma once

#include <type_traits>

namespace util {

template<typename T>
struct Point2D {
    T x{}, y{};

    Point2D(T x, T y) : x(x), y(y) { }

    template<typename S>
    Point2D<std::common_type_t<T, S>> operator+(const Point2D<S>& other) const {
        return Point2D<std::common_type_t<T, S>>(x + other.x, y + other.y);
    }
};

template<typename T>
struct Point3D {
    T x{}, y{}, z{};

    Point3D(T x, T y, T z) : x(x), y(y), z(z) { }

    template<typename S>
    Point3D<std::common_type_t<T, S>> operator+(const Point3D<S>& other) const {
        return Point3D<std::common_type_t<T, S>>(x + other.x, y + other.y, z + other.z);
    }
};

}