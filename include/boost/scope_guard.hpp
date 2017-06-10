
//                     Copyright Yuri Kilochek 2017.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_HPP
#define BOOST_SCOPE_GUARD_HPP

#if __cplusplus < 201703L
    #error Boost.ScopeGuard requires C++17 or later.
#endif

#include <functional>
#include <type_traits>
#include <utility>
#include <tuple>
#include <exception>

#include <boost/config.hpp>

namespace boost {
///////////////////////////////////////////////////////////////////////////////

namespace detail::scope_guard {
    template <typename T>
    struct unwrap
    { using type = T; };

    template <typename T>
    struct unwrap<std::reference_wrapper<T>>
    { using type = T&; };

    template <typename T>
    using unwrap_decay_t = typename unwrap<std::decay_t<T>>::type;

    template <typename Fn, typename... Args>
    struct base
    {
        static_assert(std::is_invocable_v<
            unwrap_decay_t<Fn>, unwrap_decay_t<Args>...>);

        base(Fn&& fn, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<unwrap_decay_t<Fn>, Fn> &&
                (std::is_nothrow_constructible_v<unwrap_decay_t<Args>, Args> &&
                 ...))
        : fn(std::forward<Fn>(fn))
        , args(std::forward<Args>(args)...)
        {}

        base() = delete;

        base(base const&) = delete;
        auto operator=(base const&)
        -> base&
        = delete;

        base(base&&) = delete;
        auto operator=(base&&)
        -> base&
        = delete;

    protected:
        void invoke()
        noexcept(noexcept(std::invoke(
            std::declval<unwrap_decay_t<Fn>>(),
            std::declval<unwrap_decay_t<Args>>()...)))
        { std::apply(std::forward<unwrap_decay_t<Fn>>(fn), std::move(args)); }

    private:
        unwrap_decay_t<Fn> fn;
        std::tuple<unwrap_decay_t<Args>...> args;
    };
} // detail::scope_guard

template <typename Fn, typename... Args>
class scope_guard final
: detail::scope_guard::base<Fn, Args...>
{
    using base_type = detail::scope_guard::base<Fn, Args...>;

public:
    using base_type::base_type;

    ~scope_guard()
    noexcept(noexcept(std::declval<scope_guard>().invoke()) &&
             std::is_nothrow_destructible_v<base_type>)
    { base_type::invoke(); }
};

template <typename Fn, typename... Args>
scope_guard(Fn&&, Args&&...)
-> scope_guard<Fn, Args...>;


template <typename Fn, typename... Args>
class scope_guard_failure final
: detail::scope_guard::base<Fn, Args...>
{
    using base_type = detail::scope_guard::base<Fn, Args...>;

public:
    using base_type::base_type;

    ~scope_guard_failure()
    noexcept(noexcept(std::declval<scope_guard_failure>().invoke()) &&
             std::is_nothrow_destructible_v<base_type>)
    {
        int const out = std::uncaught_exceptions();
        if (BOOST_UNLIKELY(out > in)) { base_type::invoke(); }
    }

private:
    int const in = std::uncaught_exceptions();
};

template <typename Fn, typename... Args>
scope_guard_failure(Fn&&, Args&&...)
-> scope_guard_failure<Fn, Args...>;


template <typename Fn, typename... Args>
class scope_guard_success final
: detail::scope_guard::base<Fn, Args...>
{
    using base_type = detail::scope_guard::base<Fn, Args...>;

public:
    using base_type::base_type;

    ~scope_guard_success()
    noexcept(noexcept(std::declval<scope_guard_success>().invoke()) &&
             std::is_nothrow_destructible_v<base_type>)
    {
        int const out = std::uncaught_exceptions();
        if (BOOST_LIKELY(out == in)) { base_type::invoke(); }
    }

private:
    int const in = std::uncaught_exceptions();
};

template <typename Fn, typename... Args>
scope_guard_success(Fn&&, Args&&...)
-> scope_guard_success<Fn, Args...>;

///////////////////////////////////////////////////////////////////////////////
} // boost

#endif

