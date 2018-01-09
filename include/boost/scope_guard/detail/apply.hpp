
//                     Copyright Yuri Kilochek 2018.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_DETAIL_APPLY_HPP
#define BOOST_SCOPE_GUARD_DETAIL_APPLY_HPP

#include <boost/scope_guard/detail/fn_alias.hpp>

#include <cstddef>
#include <utility>
#include <functional>
#include <tuple>
#include <type_traits>

namespace boost::detail::scope_guard {
///////////////////////////////////////////////////////////////////////////////

template <std::size_t... I, typename Fn, typename Args>
constexpr
auto apply_impl(std::index_sequence<I...>, Fn&& fn, Args&& args)
BOOST_DETAIL_SCOPE_GUARD_FN_ALIAS(std::invoke(
    std::forward<Fn>(fn), std::get<I>(std::forward<Args>(args))...))
 
// Like `std::apply` but SFINAE friendly and propagates `noexcept`ness.
template <class Fn, typename Args>
constexpr
auto apply(Fn&& fn, Args&& args)
BOOST_DETAIL_SCOPE_GUARD_FN_ALIAS(apply_impl(
    std::make_index_sequence<
        std::tuple_size_v<std::remove_reference_t<Args>>>{},
    std::forward<Fn>(fn), std::forward<Args>(args)))

///////////////////////////////////////////////////////////////////////////////
} // boost::detail::scope_guard

#endif

