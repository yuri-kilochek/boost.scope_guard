
//                     Copyright Yuri Kilochek 2018.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_DETAIL_ACTION_HPP
#define BOOST_SCOPE_GUARD_DETAIL_ACTION_HPP

#include <boost/scope_guard/detail/fn_alias.hpp>
#include <boost/scope_guard/detail/apply.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace boost::detail::scope_guard {
///////////////////////////////////////////////////////////////////////////////

template <typename Fn, typename... Args>
class action
{
    Fn fn_; std::tuple<Args...> args_;

public:
    template <typename Fn_, typename... Args_, std::enable_if_t<
        std::is_constructible_v<Fn, Fn_> &&
        (std::is_constructible_v<Args, Args_> && ...)>*...>
    constexpr explicit
    action(Fn_&& fn, Args_&&... args)
    noexcept((std::is_nothrow_constructible_v<Fn, Fn_> && ... &&
              std::is_nothrow_constructible_v<Args, Args_>))
    : fn_(std::forward<Fn_>(fn)), args_(std::forward<Args_>(args)...)
    {}

    ~action() noexcept = default;

    constexpr
    auto operator()()
    BOOST_DETAIL_SCOPE_GUARD_FN_ALIAS(
        (void)detail::scope_guard::apply(
            std::forward<Fn>(fn_), std::move(args_)))
};

///////////////////////////////////////////////////////////////////////////////
} // boost::detail::scope_guard

#endif

