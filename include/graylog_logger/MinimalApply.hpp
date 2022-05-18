/* Copyright (C) 2020 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Some functions from C++17 for using std::tuple with variadic
/// functions.
///
//===----------------------------------------------------------------------===//

#include <tuple>

namespace minimal {
template <class F, class... Args>
constexpr decltype(auto) invoke(F &&f, Args &&...args) {
  return std::forward<F>(f)(std::forward<Args>(args)...);
}

namespace detail {
template <class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F &&f, Tuple &&t,
                                    std::index_sequence<I...>) {
  return minimal::invoke(std::forward<F>(f),
                         std::get<I>(std::forward<Tuple>(t))...);
}
} // namespace detail

template <class F, class Tuple>
constexpr decltype(auto) apply(F &&f, Tuple &&t) {
  return detail::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>());
}
} // namespace minimal