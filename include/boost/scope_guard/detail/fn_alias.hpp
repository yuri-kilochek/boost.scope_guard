
//                     Copyright Yuri Kilochek 2018.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_DETAIL_FN_ALIAS_HPP
#define BOOST_SCOPE_GUARD_DETAIL_FN_ALIAS_HPP

///////////////////////////////////////////////////////////////////////////////

#define BOOST_DETAIL_SCOPE_GUARD_FN_ALIAS(...) \
    noexcept(noexcept(__VA_ARGS__)) \
    -> decltype(__VA_ARGS__) \
    { return __VA_ARGS__; }

///////////////////////////////////////////////////////////////////////////////

#endif

