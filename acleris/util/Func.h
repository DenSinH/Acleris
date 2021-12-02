#pragma once

#include <type_traits>
#include <functional>
#include <tuple>


namespace util {

/*
 * For getting function argument types.
 * */

template<typename Callable> struct func;

template<typename R, typename... Args>
struct func<R(Args...)> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename R, typename... Args>
struct func<R (*)(Args...)> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename R, typename C, typename... Args>
struct func<R (C::*)(Args...)> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename R, typename C, typename... Args>
struct func<R (C::*)(Args...) const> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename Callable>
struct func {
    using args_t = typename func<decltype(&Callable::operator())>::args_t;
    using return_t = typename func<decltype(&Callable::operator())>::return_t;
};

template<class F>
using func_args_t = typename func<F>::args_t;
template<class F>
using func_return_t = typename func<F>::return_t;

}