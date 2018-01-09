
//                     Copyright Yuri Kilochek 2018.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_HPP
#define BOOST_SCOPE_GUARD_HPP

#if __cplusplus < 201703L
    #error Boost.ScopeGuard requires C++17 or later.
#endif

#include <boost/scope_guard/detail/base.hpp>
#include <boost/scope_guard/detail/unwrap_decay.hpp>
#include <boost/scope_guard/detail/unique_name.hpp>

#include <boost/config.hpp>

#include <type_traits>
#include <exception>

namespace boost {
///////////////////////////////////////////////////////////////////////////////

template <typename... Params>
class scope_guard
: detail::scope_guard::base<Params...>
{
    using base_type = detail::scope_guard::base<Params...>;

public:
    using base_type::base_type;

    ~scope_guard()
    noexcept(std::is_nothrow_invocable_v<typename base_type::action_type>)
    { this->action_(); }
};

template <typename... Params>
scope_guard(Params&&...)
-> scope_guard<detail::scope_guard::unwrap_decay_t<Params>...>;


template <typename... Params>
class scope_guard_failure
: detail::scope_guard::base<Params...>
{
    using base_type = detail::scope_guard::base<Params...>;

    int in = std::uncaught_exceptions();

public:
    using base_type::base_type;

    ~scope_guard_failure()
    noexcept(std::is_nothrow_invocable_v<typename base_type::action_type>)
    {
        int out = std::uncaught_exceptions();
        if (BOOST_UNLIKELY(out > in)) { this->action_(); }
    }
};

template <typename... Params>
scope_guard_failure(Params&&...)
-> scope_guard_failure<detail::scope_guard::unwrap_decay_t<Params>...>;


template <typename... Params>
class scope_guard_success
: detail::scope_guard::base<Params...>
{
    using base_type = detail::scope_guard::base<Params...>;

    int in = std::uncaught_exceptions();

public:
    using base_type::base_type;

    ~scope_guard_success()
    noexcept(std::is_nothrow_invocable_v<typename base_type::action_type>)
    {
        int out = std::uncaught_exceptions();
        if (BOOST_LIKELY(out == in)) { this->action_(); }
    }
};

template <typename... Params>
scope_guard_success(Params&&...)
-> scope_guard_success<detail::scope_guard::unwrap_decay_t<Params>...>;

///////////////////////////////////////////////////////////////////////////////
} // boost

#define BOOST_SCOPE_GUARD \
    ::boost::scope_guard BOOST_DETAIL_SCOPE_GUARD_UNIQUE_NAME =

#define BOOST_SCOPE_GUARD_FAILURE \
    ::boost::scope_guard_failure BOOST_DETAIL_SCOPE_GUARD_UNIQUE_NAME =

#define BOOST_SCOPE_GUARD_SUCCESS \
    ::boost::scope_guard_success BOOST_DETAIL_SCOPE_GUARD_UNIQUE_NAME =

#endif

