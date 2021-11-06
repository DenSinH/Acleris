#pragma once

#include <type_traits>
#include <functional>


namespace util {

template<class Sig>
struct func {
    using type = typename func<decltype(&Sig::operator())>::type;
    using return_t = typename func<decltype(&Sig::operator())>::return_t;
};

template<class R, class...Args>
struct func<R(Args...)> {
    using type = std::tuple<Args...>;
    using return_t = R;
};

template<class R, class...Args>
struct func<std::function<R(Args...)>> {
    using type = std::tuple<Args...>;
    using return_t = R;
};

template<typename F, typename R, typename... Args>
struct func<R (F::*)(Args...)> {
    using type = std::tuple<Args...>;
    using return_t = R;
};

template<typename F, typename R, typename... Args>
struct func<R (F::*)(Args...) const> {
    using type = std::tuple<Args...>;
    using return_t = R;
};

template<class Sig>
using func_args_t = typename func<Sig>::type;
template<class Sig>
using func_return_t = typename func<Sig>::return_t;

}