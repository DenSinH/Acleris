#pragma once

#include <type_traits>
#include <utility>
#include <vector>
#include <array>
#include <memory>
#include <numeric>
#include <tuple>
#include <cstdio>

#include "StaticFor.h"


namespace util {

template<typename T, size_t s, size_t... sizes>
struct NArray {
    using base_t = std::array<std::conditional<sizeof...(sizes) == 0, NArray<T, sizes...>, T>, s>;
    base_t base;

    base_t& operator[](size_t pos) { return base[pos]; }
    const base_t& operator[](size_t pos) const { return base[pos]; }
    base_t& at(size_t pos) { return base.at(pos); }
    const base_t& at(size_t pos) const { return base.at(pos); }
    T* data() { return base.data(); }
    const T* data() const { return base.data(); }
    static size_t size() { return base_t::size() * s; }
};


template<typename T, int n, bool transpose = true>
struct NVect {
    static_assert(n > 0);
    using stride_t = std::array<size_t, n + 1>;

    std::vector<T> base;
    std::array<size_t, n + 1> stride;

    template<typename... Args>
    NVect(Args... args) :
            base{},
            stride{transpose ? stride_t{1, (size_t)args...} : stride_t{(size_t)args..., 1}} {
        for (int i = 0; i < n; i++) {
            if constexpr(transpose) {
                stride[i + 1] *= stride[i];
            }
            else {
                stride[n + 1 - i] *= stride[n - i];
            }
        }

        base.resize(stride[transpose ? n : 0]);
    }

    template<typename... Args>
    T& operator()(Args... args) {
        static_assert(sizeof...(Args) == n);
        size_t offs = 0;
        util::static_for<size_t, 0, n>([&](auto i) {
            offs += stride[i] * std::get<i>(std::make_tuple(args...));
        });
        return base[offs];
    }

    template<typename... Args>
    const T& operator()(Args... args) const { this(args...); }

    T* data() { return base.data(); }
    const T* data() const { return base.data(); }
    size_t size() { return base.size(); }

    typename std::vector<T>::iterator begin() { return base.begin(); }
    typename std::vector<T>::const_iterator begin() const { return base.begin(); }
    typename std::vector<T>::iterator end() { return base.end(); }
    typename std::vector<T>::const_iterator end() const { return base.end(); }
};

}