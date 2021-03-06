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


#ifndef LIBCPPA_UTIL_TYPE_LIST_HPP
#define LIBCPPA_UTIL_TYPE_LIST_HPP

#include <typeinfo>

#include "cppa/util/if_else.hpp"
#include "cppa/util/type_pair.hpp"
#include "cppa/util/void_type.hpp"

namespace cppa {

// forward declarations
class uniform_type_info;
uniform_type_info const* uniform_typeid(std::type_info const&);

} // namespace cppa

namespace cppa { namespace util {

template<typename... Types> struct type_list;

template<>
struct type_list<>
{
    typedef void_type head;
    typedef void_type back;
    typedef type_list<> tail;
    static const size_t size = 0;
};

template<typename Head, typename... Tail>
struct type_list<Head, Tail...>
{

    typedef Head head;

    typedef type_list<Tail...> tail;

    typedef typename if_else_c<sizeof...(Tail) == 0,
                               Head,
                               wrapped<typename tail::back> >::type
            back;

    static const size_t size =  sizeof...(Tail) + 1;

};

// static list list::zip(list, list)

/**
 * @brief Zips two lists of equal size.
 *
 * Creates a list formed from the two lists @p ListA and @p ListB,
 * e.g., tl_zip<type_list<int,double>,type_list<float,string>>::type
 * is type_list<type_pair<int,float>,type_pair<double,string>>.
 */
template<class ListA, class ListB>
struct tl_zip;

template<typename... LhsElements, typename... RhsElements>
struct tl_zip<type_list<LhsElements...>, type_list<RhsElements...> >
{
    static_assert(type_list<LhsElements...>::size ==
                  type_list<RhsElements...>::size,
                  "Lists have different size");
    typedef type_list<type_pair<LhsElements, RhsElements>...> type;
};

// list list::reverse()

template<class From, typename... Elements>
struct tl_reverse_impl;

template<typename T0, typename... T, typename... E>
struct tl_reverse_impl<type_list<T0, T...>, E...>
{
    typedef typename tl_reverse_impl<type_list<T...>, T0, E...>::type type;
};

template<typename... E>
struct tl_reverse_impl<type_list<>, E...>
{
    typedef type_list<E...> type;
};

/**
 * @brief Creates a new list wih elements in reversed order.
 */
template<class List>
struct tl_reverse
{
    typedef typename tl_reverse_impl<List>::type type;
};

// bool list::find(type)

/**
 * @brief Finds the first element of type @p What beginning at
 *        index @p Pos.
 */
template<class List, typename What, int Pos = 0>
struct tl_find;

template<typename What, int Pos>
struct tl_find<type_list<>, What, Pos>
{
    static constexpr int value = -1;
    typedef type_list<> rest_list;
};

template<typename What, int Pos, typename... Tail>
struct tl_find<type_list<What, Tail...>, What, Pos>
{
    static constexpr int value = Pos;
    typedef type_list<Tail...> rest_list;
};

template<typename What, int Pos, typename Head, typename... Tail>
struct tl_find<type_list<Head, Tail...>, What, Pos>
{
    static constexpr int value = tl_find<type_list<Tail...>, What, Pos+1>::value;
    typedef typename tl_find<type_list<Tail...>, What, Pos+1>::rest_list rest_list;
};

// list list::first_n(size_t)

template<size_t N, class List, typename... T>
struct tl_first_n_impl;

template<class List, typename... T>
struct tl_first_n_impl<0, List, T...>
{
    typedef type_list<T...> type;
};

template<size_t N, typename L0, typename... L, typename... T>
struct tl_first_n_impl<N, type_list<L0, L...>, T...>
{
    typedef typename tl_first_n_impl<N-1, type_list<L...>, T..., L0>::type type;
};

/**
 * @brief Creates a new list from the first @p N elements of @p List.
 */
template<class List, size_t N>
struct tl_first_n
{
    static_assert(N > 0, "N == 0");
    static_assert(List::size >= N, "List::size < N");
    typedef typename tl_first_n_impl<N, List>::type type;
};

// bool list::forall(predicate)

/**
 * @brief Tests whether a predicate holds for all elements of a list.
 */
template<class List, template <typename> class Predicate>
struct tl_forall
{
    static constexpr bool value =
               Predicate<typename List::head>::value
            && tl_forall<typename List::tail, Predicate>::value;
};

template<template <typename> class Predicate>
struct tl_forall<type_list<>, Predicate>
{
    static constexpr bool value = true;
};

/**
 * @brief Tests whether a predicate holds for some of the elements of a list.
 */
template<class List, template <typename> class Predicate>
struct tl_exists
{
    static constexpr bool value =
               Predicate<typename List::head>::value
            || tl_exists<typename List::tail, Predicate>::value;
};

template<template <typename> class Predicate>
struct tl_exists<type_list<>, Predicate>
{
    static constexpr bool value = false;
};


// size_t list::count(predicate)

/**
 * @brief Counts the number of elements in the list which satisfy a predicate.
 */
template<class List, template <typename> class Predicate>
struct tl_count
{
    static constexpr size_t value =
              (Predicate<typename List::head>::value ? 1 : 0)
            + tl_count<typename List::tail, Predicate>::value;
};

template<template <typename> class Predicate>
struct tl_count<type_list<>, Predicate>
{
    static constexpr size_t value = 0;
};

// size_t list::count_not(predicate)

/**
 * @brief Counts the number of elements in the list which satisfy a predicate.
 */
template<class List, template <typename> class Predicate>
struct tl_count_not
{
    static constexpr size_t value =
              (Predicate<typename List::head>::value ? 0 : 1)
            + tl_count_not<typename List::tail, Predicate>::value;
};

template<template <typename> class Predicate>
struct tl_count_not<type_list<>, Predicate>
{
    static constexpr size_t value = 0;
};

// bool list::zipped_forall(predicate)

/**
 * @brief Tests whether a predicate holds for all elements of a zipped list.
 */
template<class ZippedList, template <typename, typename> class Predicate>
struct tl_zipped_forall
{
    typedef typename ZippedList::head head;
    static constexpr bool value =
               Predicate<typename head::first, typename head::second>::value
            && tl_zipped_forall<typename ZippedList::tail, Predicate>::value;
};

template<template <typename, typename> class Predicate>
struct tl_zipped_forall<type_list<>, Predicate>
{
    static constexpr bool value = true;
};

// static list list::concat(list, list)

/**
 * @brief Concatenates two lists.
 */
template<typename ListA, typename ListB>
struct tl_concat;

template<typename... ListATypes, typename... ListBTypes>
struct tl_concat<type_list<ListATypes...>, type_list<ListBTypes...> >
{
    typedef type_list<ListATypes..., ListBTypes...> type;
};

// list list::appy(trait)

/**
 * @brief Applies a "template function" to each element in the list.
 */
template<typename List, template <typename> class Trait>
struct tl_apply;

template<template <typename> class Trait, typename... Elements>
struct tl_apply<type_list<Elements...>, Trait>
{
    typedef type_list<typename Trait<Elements>::type...> type;
};

// list list::zipped_apply(trait)

/**
 * @brief Applies a binary "template function" to each element
 *        in the zipped list.
 */
template<typename List, template<typename, typename> class Trait>
struct tl_zipped_apply;

template<template<typename, typename> class Trait, typename... T>
struct tl_zipped_apply<type_list<T...>, Trait>
{
    typedef type_list<typename Trait<typename T::first,
                                     typename T::second>::type...> type;
};

// list list::pop_back()

/**
 * @brief Creates a new list wih all but the last element of @p List.
 */
template<class List>
struct tl_pop_back
{
    typedef typename tl_reverse<List>::type rlist;
    typedef typename tl_reverse<typename rlist::tail>::type type;
};

// type list::at(size_t)

template<size_t N, typename... E>
struct tl_at_impl;


template<size_t N, typename E0, typename... E>
struct tl_at_impl<N, E0, E...>
{
    typedef typename tl_at_impl<N-1, E...>::type type;
};

template<typename E0, typename... E>
struct tl_at_impl<0, E0, E...>
{
    typedef E0 type;
};

/**
 * @brief Gets element at index @p N of @p List.
 */
template<class List, size_t N>
struct tl_at;

template<size_t N, typename... E>
struct tl_at<type_list<E...>, N>
{
    static_assert(N < sizeof...(E), "N >= List::size");
    typedef typename tl_at_impl<N, E...>::type type;
};

// list list::prepend(type)

/**
 * @brief Creates a new list with @p What prepended to @p List.
 */
template<class List, typename What>
struct tl_prepend;

template<typename What, typename... T>
struct tl_prepend<type_list<T...>, What>
{
    typedef type_list<What, T...> type;
};


// list list::filter(predicate)
// list list::filter_not(predicate)

template<class List, bool... Selected>
struct tl_filter_impl;

template<>
struct tl_filter_impl< type_list<> >
{
    typedef type_list<> type;
};

template<typename T0, typename... T, bool... S>
struct tl_filter_impl<type_list<T0, T...>, false, S...>
{
    typedef typename tl_filter_impl<type_list<T...>, S...>::type type;
};

template<typename T0, typename... T, bool... S>
struct tl_filter_impl<type_list<T0, T...>, true, S...>
{
    typedef typename tl_prepend<typename tl_filter_impl<type_list<T...>, S...>::type, T0>::type type;
};

/**
 * @brief Create a new list containing all elements which satisfy @p Predicate.
 */
template<class List, template<typename> class Predicate>
struct tl_filter;

template<template<typename> class Predicate, typename... T>
struct tl_filter<type_list<T...>, Predicate>
{
    typedef typename tl_filter_impl<type_list<T...>, Predicate<T>::value...>::type type;
};

/**
 * @brief Create a new list containing all elements which
 *        do not satisfy @p Predicate.
 */
template<class List, template<typename> class Predicate>
struct tl_filter_not;

template<template<typename> class Predicate, typename... T>
struct tl_filter_not<type_list<T...>, Predicate>
{
    typedef typename tl_filter_impl<type_list<T...>, !Predicate<T>::value...>::type type;
};

} } // namespace cppa::util

#endif // LIBCPPA_UTIL_TYPE_LIST_HPP
