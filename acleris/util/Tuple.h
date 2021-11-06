#pragma once


#include <tuple>

namespace util {

template<typename... T, std::size_t... I>
constexpr auto slice_tuple_impl(std::tuple<T...> tuple, std::index_sequence<I...>) {
    return std::tuple<typename std::tuple_element_t<I, std::tuple<T...>>...>{std::get<I>(tuple)...};
}

template <int n, typename... Ts>
auto slice_tuple(const std::tuple<Ts...>& tuple) {
    return slice_tuple_impl(tuple, std::make_index_sequence<n>());
}

template<typename... Ts>
size_t tuple_size(const std::tuple<Ts...>& tuple) {
    return sizeof...(Ts);
}

}