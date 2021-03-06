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


#ifndef DURATION_HPP
#define DURATION_HPP

#include <chrono>
#include <cstdint>

namespace cppa { namespace util {

enum class time_unit : std::uint32_t
{
    none = 0,
    seconds = 1,
    milliseconds = 1000,
    microseconds = 1000000
};

template<typename Period>
constexpr time_unit get_time_unit_from_period()
{
    return (Period::num != 1
            ? time_unit::none
            : (Period::den == 1
               ? time_unit::seconds
               : (Period::den == 1000
                  ? time_unit::milliseconds
                  : (Period::den == 1000000
                     ? time_unit::microseconds
                     : time_unit::none))));
}

class duration
{

 public:

    constexpr duration() : unit(time_unit::none), count(0)
    {
    }

    constexpr duration(time_unit un, std::uint32_t val) : unit(un), count(val)
    {
    }

    template <class Rep, class Period>
    constexpr duration(std::chrono::duration<Rep, Period> d)
        : unit(get_time_unit_from_period<Period>()), count(d.count())
    {
        static_assert(get_time_unit_from_period<Period>() != time_unit::none,
                      "only seconds, milliseconds or microseconds allowed");
    }

    time_unit unit;

    std::uint32_t count;

};

bool operator==(duration const& lhs, duration const& rhs);

inline bool operator!=(duration const& lhs, duration const& rhs)
{
    return !(lhs == rhs);
}

} } // namespace cppa::util

template<class Clock, class Duration>
std::chrono::time_point<Clock, Duration>&
operator+=(std::chrono::time_point<Clock, Duration>& lhs,
           cppa::util::duration const& rhs)
{
    switch (rhs.unit)
    {
        case cppa::util::time_unit::seconds:
            lhs += std::chrono::seconds(rhs.count);
            break;

        case cppa::util::time_unit::milliseconds:
            lhs += std::chrono::milliseconds(rhs.count);
            break;

        case cppa::util::time_unit::microseconds:
            lhs += std::chrono::microseconds(rhs.count);
            break;

        default: break;
    }
    return lhs;
}

#endif // DURATION_HPP
