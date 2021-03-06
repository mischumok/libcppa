/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011, 2012                                                   *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation, either version 3 of the License                  *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#ifndef TUPLE_CAST_HPP
#define TUPLE_CAST_HPP

#include <type_traits>

#include "cppa/match.hpp"
#include "cppa/option.hpp"
#include "cppa/pattern.hpp"

#include "cppa/detail/tuple_cast_impl.hpp"

namespace cppa {

// cast using a pattern
template<typename... T>
auto tuple_cast(any_tuple const& tup, pattern<T...> const& p)
    -> option<
        typename tuple_from_type_list<
            typename pattern<T...>::filtered_types
        >::type>
{
    typedef typename pattern<T...>::filtered_types filtered_types;
    typedef typename tuple_from_type_list<filtered_types>::type tuple_type;
    static constexpr auto impl =
            get_pattern_characteristic<util::type_list<T...>>();
    return detail::tuple_cast_impl<impl, tuple_type, T...>::_(tup, p);
}

// cast using types
template<typename... T>
auto tuple_cast(any_tuple const& tup)
    -> option<
        typename tuple_from_type_list<
            typename util::tl_filter_not<
                util::type_list<T...>,
                util::tbind<std::is_same, anything>::type
            >::type
        >::type>
{
    typedef decltype(tuple_cast<T...>(tup)) result_type;
    typedef typename result_type::value_type tuple_type;
    static constexpr auto impl =
            get_pattern_characteristic<util::type_list<T...>>();
    return detail::tuple_cast_impl<impl, tuple_type, T...>::_(tup);
}

} // namespace cppa

#endif // TUPLE_CAST_HPP
