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


#ifndef CPPA_TUPLE_HPP
#define CPPA_TUPLE_HPP

#include <cstddef>
#include <string>
#include <typeinfo>
#include <type_traits>

#include "cppa/get.hpp"
#include "cppa/actor.hpp"
#include "cppa/cow_ptr.hpp"
#include "cppa/ref_counted.hpp"

#include "cppa/util/at.hpp"
#include "cppa/util/fixed_vector.hpp"
#include "cppa/util/replace_type.hpp"
#include "cppa/util/is_comparable.hpp"
#include "cppa/util/compare_tuples.hpp"
#include "cppa/util/is_legal_tuple_type.hpp"

#include "cppa/detail/tuple_vals.hpp"
#include "cppa/detail/decorated_tuple.hpp"
#include "cppa/detail/implicit_conversions.hpp"

namespace cppa {

// forward declaration
class any_tuple;
class local_actor;

/**
 * @ingroup CopyOnWrite
 * @brief A fixed-length copy-on-write tuple.
 */
template<typename... ElementTypes>
class tuple
{

    static_assert(sizeof...(ElementTypes) > 0, "tuple is empty");

    static_assert(util::tl_forall<util::type_list<ElementTypes...>,
                                  util::is_legal_tuple_type>::value,
                  "illegal types in tuple definition: "
                  "pointers and references are prohibited");

    friend class any_tuple;

    typedef detail::tuple_vals<ElementTypes...> data_type;
    typedef detail::decorated_tuple<ElementTypes...> decorated_type;

    cow_ptr<detail::abstract_tuple> m_vals;

    struct priv_ctor { };

    tuple(priv_ctor, cow_ptr<detail::abstract_tuple>&& ptr) : m_vals(std::move(ptr)) { }

 public:

    typedef util::type_list<ElementTypes...> types;
    typedef cow_ptr<detail::abstract_tuple> cow_ptr_type;

    static constexpr size_t num_elements = sizeof...(ElementTypes);

    /**
     * @brief Initializes each element with its default constructor.
     */
    tuple() : m_vals(new data_type)
    {
    }

    /**
     * @brief Initializes the tuple with @p args.
     * @param args Initialization values.
     */
    tuple(ElementTypes const&... args) : m_vals(new data_type(args...))
    {
    }

    /**
     * @brief Initializes the tuple with @p args.
     * @param args Initialization values.
     */
    tuple(ElementTypes&&... args) : m_vals(new data_type(std::move(args)...))
    {
    }

    tuple(tuple&&) = default;
    tuple(tuple const&) = default;
    tuple& operator=(tuple&&) = default;
    tuple& operator=(tuple const&) = default;

    inline static tuple from(cow_ptr_type ptr)
    {
        return {priv_ctor(), std::move(ptr)};
    }

    inline static tuple from(cow_ptr_type ptr,
                             util::fixed_vector<size_t, num_elements> const& mv)
    {
        return {priv_ctor(), decorated_type::create(std::move(ptr), mv)};
    }

    inline static tuple offset_subtuple(cow_ptr_type ptr, size_t offset)
    {
        CPPA_REQUIRE(offset > 0);
        return {priv_ctor(), decorated_type::create(std::move(ptr), offset)};
    }

    /**
     * @brief Gets the size of this tuple.
     */
    inline size_t size() const
    {
        return sizeof...(ElementTypes);
    }

    /**
     * @brief Gets a const pointer to the element at position @p p.
     */
    inline void const* at(size_t p) const
    {
        return m_vals->at(p);
    }

    /**
     * @brief Gets a mutable pointer to the element at position @p p.
     */
    inline void* mutable_at(size_t p)
    {
        return m_vals->mutable_at(p);
    }

    /**
     * @brief Gets {@link uniform_type_info uniform type information}
     *        of the element at position @p p.
     */
    inline uniform_type_info const* type_at(size_t p) const
    {
        return m_vals->type_at(p);
    }

    inline cow_ptr<detail::abstract_tuple> const& vals() const
    {
        return m_vals;
    }

};

template<typename TypeList>
struct tuple_from_type_list;

template<typename... Types>
struct tuple_from_type_list< util::type_list<Types...> >
{
    typedef tuple<Types...> type;
};

#ifdef CPPA_DOCUMENTATION

/**
 * @ingroup CopyOnWrite
 * @brief Gets a const-reference to the <tt>N</tt>th element of @p tup.
 * @param tup The tuple object.
 * @returns A const-reference of type T, whereas T is the type of the
 *          <tt>N</tt>th element of @p tup.
 * @relates tuple
 */
template<size_t N, typename T>
T const& get(tuple<...> const& tup);

/**
 * @ingroup CopyOnWrite
 * @brief Gets a reference to the <tt>N</tt>th element of @p tup.
 * @param tup The tuple object.
 * @returns A reference of type T, whereas T is the type of the
 *          <tt>N</tt>th element of @p tup.
 * @note Detaches @p tup if there are two or more references to the tuple data.
 * @relates tuple
 */
template<size_t N, typename T>
T& get_ref(tuple<...>& tup);

/**
 * @ingroup ImplicitConversion
 * @brief Creates a new tuple from @p args.
 * @param args Values for the tuple elements.
 * @returns A tuple object containing the values @p args.
 * @relates tuple
 */
template<typename... Types>
tuple<Types...> make_tuple(Types const&... args);

#else

template<size_t N, typename... Types>
const typename util::at<N, Types...>::type& get(tuple<Types...> const& tup)
{
    typedef typename util::at<N, Types...>::type result_type;
    return *reinterpret_cast<result_type const*>(tup.at(N));
}

template<size_t N, typename... Types>
typename util::at<N, Types...>::type& get_ref(tuple<Types...>& tup)
{
    typedef typename util::at<N, Types...>::type result_type;
    return *reinterpret_cast<result_type*>(tup.mutable_at(N));
}

template<typename... Types>
typename tuple_from_type_list<
    typename util::tl_apply<util::type_list<Types...>,
                            detail::implicit_conversions>::type>::type
make_tuple(Types const&... args)
{
    return { args... };
}

#endif

/**
 * @brief Compares two tuples.
 * @param lhs First tuple object.
 * @param rhs Second tuple object.
 * @returns @p true if @p lhs and @p rhs are equal; otherwise @p false.
 * @relates tuple
 */
template<typename... LhsTypes, typename... RhsTypes>
inline bool operator==(tuple<LhsTypes...> const& lhs,
                       tuple<RhsTypes...> const& rhs)
{
    return util::compare_tuples(lhs, rhs);
}

/**
 * @brief Compares two tuples.
 * @param lhs First tuple object.
 * @param rhs Second tuple object.
 * @returns @p true if @p lhs and @p rhs are not equal; otherwise @p false.
 * @relates tuple
 */
template<typename... LhsTypes, typename... RhsTypes>
inline bool operator!=(tuple<LhsTypes...> const& lhs,
                       tuple<RhsTypes...> const& rhs)
{
    return !(lhs == rhs);
}

} // namespace cppa

#endif
