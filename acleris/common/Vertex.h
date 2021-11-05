#pragma once

#include <array>
#include <tuple>


template<typename T, int n, typename... Args>
struct Vertex {
    static constexpr int dim = n;
    static constexpr int no_args = sizeof...(Args);
    using type = T;

    std::array<T, n> x{};
    const std::tuple<Args...> args;

    Vertex(std::array<T, n> x, Args... args) :
            x{std::move(x)}, args{args...} {

    }
};


template<typename T, typename V0, typename V1, typename V2>
std::pair<T, T> BarycentricInterp(T x, T y, const V0& v0, const V1& v1, const V2& v2) {
    static_assert(V0::dim == 2);
    static_assert(V1::dim == 2);
    static_assert(V2::dim == 2);
    static_assert(V0::no_args == V1::no_args);
    static_assert(V0::no_args == V2::no_args);

    // barycentric coordinates: https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Edge_approach
    const T det = T(v1.x[1] - v2.x[1]) * T(v0.x[0] - v2.x[0]) + T(v2.x[0] - v1.x[0]) * T(v0.x[1] - v2.x[1]);
    const T l0 = T(v1.x[1] - v2.x[1]) * T(x - v2.x[0]) + T(v2.x[0] - v1.x[0]) * T(y - v2.x[1]);
    const T l1 = T(v2.x[1] - v0.x[1]) * T(x - v2.x[0]) + T(v0.x[0] - v2.x[0]) * T(y - v2.x[1]);
    return std::make_pair(l0 / det, l1 / det);
}


template<typename V0, typename V1, typename V2>
struct Triangle {
    const V0& v0;
    const V1& v1;
    const V2& v2;

    Triangle(const V0& v0, const V1& v1, const V2& v2) : v0(v0), v1(v1), v2(v2) { }


    template<typename F, typename T = std::common_type_t<std::common_type_t<typename V0::type, typename V1::type>, typename V2::type>>
    std::uint32_t Fragment(T x, T y, F func) {
        static_assert(V0::dim == 2);
        static_assert(V1::dim == 2);
        static_assert(V2::dim == 2);
        static_assert(V0::no_args == V1::no_args);
        static_assert(V0::no_args == V2::no_args);

        auto l = BarycentricInterp(x, y, v0, v1, v2);
        const T l0 = l.first;
        const T l1 = l.second;
        const T l2 = 1 - l0 - l1;

        const auto op = [&](const auto& x, const auto& y, const auto& z) {
            return l0 * x + l1 * y + l2 * z;
        };

        auto args = std::apply([&](const auto&... x){
            return std::apply([&](const auto&... y){
                return std::apply([&](const auto&... z){
                    return std::make_tuple(op(x, y, z)...);
                }, v2.args);
            }, v1.args);
        }, v0.args);

        return std::apply(func, args);
    }
};


template<typename T, int n, typename... Args>
Vertex<T, n, Args...> MakeVertex(std::array<T, n> x, Args... args) {
    return Vertex<T, n, Args...>(std::move(x), args...);
}