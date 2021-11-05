#pragma once

#include <type_traits>

namespace util {

template<typename T, size_t n>
struct Point {
    static_assert(n >= 2);

    std::array<T, n> x{};

    template<typename... Args>
    Point(Args... args) : x{(T)args...} { }

    template<typename S>
    Point<std::common_type_t<T, S>, n> operator+(const Point<S, n>& other) const {
        Point<std::common_type_t<T, S>, n> result{};
        for (int i = 0; i < n; i++) {
            result.x[i] = x[i] + other[i];
        }
        return result;
    }
};

//template<typename T>
//struct Point2D {
//    T x{}, y{};
//
//    Point2D(T x, T y) : x(x), y(y) { }
//
//    template<typename S>
//    Point2D<std::common_type_t<T, S>> operator+(const Point2D<S>& other) const {
//        return Point2D<std::common_type_t<T, S>>(x + other.x, y + other.y);
//    }
//};
//
//template<typename T>
//struct Point3D {
//    T x{}, y{}, z{};
//
//    Point3D(T x, T y, T z) : x(x), y(y), z(z) { }
//
//    template<typename S>
//    Point3D<std::common_type_t<T, S>> operator+(const Point3D<S>& other) const {
//        return Point3D<std::common_type_t<T, S>>(x + other.x, y + other.y, z + other.z);
//    }
//};

}