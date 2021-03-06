
//                     Copyright Yuri Kilochek 2018.
//        Distributed under the Boost Software License, Version 1.0.
//           (See accompanying file LICENSE_1_0.txt or copy at
//                 http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SCOPE_GUARD_DETAIL_UNIQUE_NAME_HPP
#define BOOST_SCOPE_GUARD_DETAIL_UNIQUE_NAME_HPP

#include <boost/config.hpp>

///////////////////////////////////////////////////////////////////////////////

#define BOOST_DETAIL_SCOPE_GUARD_UNIQUE_NAME \
    BOOST_JOIN(boost_detail_scope_guard_in_macro_on_line_, __LINE__)

///////////////////////////////////////////////////////////////////////////////

#endif

