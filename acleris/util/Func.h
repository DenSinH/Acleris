#pragma once

#include <type_traits>
#include <functional>


namespace util {

template<class F>
struct func {
    using args_t = typename func<decltype(&F::operator())>::args_t;
    using return_t = typename func<decltype(&F::operator())>::return_t;
};

template<class R, class...Args>
struct func<R(Args...)> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<class R, class...Args>
struct func<std::function<R(Args...)>> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename C, typename R, typename... Args>
struct func<R (C::*)(Args...)> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<typename F, typename R, typename... Args>
struct func<R (F::*)(Args...) const> {
    using args_t = std::tuple<Args...>;
    using return_t = R;
};

template<class Sig>
using func_args_t = typename func<Sig>::args_t;
template<class Sig>
using func_return_t = typename func<Sig>::return_t;

}