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


#ifndef FIXED_VECTOR_HPP
#define FIXED_VECTOR_HPP

#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <initializer_list>

#include "cppa/config.hpp"

#include "cppa/config.hpp"

namespace cppa { namespace util {

/**
 * @brief A vector with a fixed maximum size.
 *
 * This implementation is highly optimized for arithmetic types and refuses
 * any non-arithmetic template parameter.
 */
template<typename T, size_t MaxSize>
class fixed_vector
{

    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

    size_t m_size;

    // support for MaxSize == 0
    T m_data[(MaxSize > 0) ? MaxSize : 1];

 public:

    typedef size_t size_type;

    typedef T& reference;
    typedef T const& const_reference;

    typedef T* iterator;
    typedef T const* const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    constexpr fixed_vector() : m_size(0) { }

    fixed_vector(fixed_vector const& other) : m_size(other.m_size)
    {
        std::copy(other.begin(), other.end(), m_data);
    }

    fixed_vector& operator=(fixed_vector const& other)
    {
        resize(other.size());
        std::copy(other.begin(), other.end(), begin());
        return *this;
    }

    void resize(size_type s)
    {
        CPPA_REQUIRE(s <= MaxSize);
        m_size = s;
    }

    fixed_vector(std::initializer_list<T> init) : m_size(init.size())
    {
        CPPA_REQUIRE(init.size() <= MaxSize);
        std::copy(init.begin(), init.end(), m_data);
    }

    inline size_type size() const
    {
        return m_size;
    }

    inline void clear()
    {
        m_size = 0;
    }

    inline bool empty() const
    {
        return m_size == 0;
    }

    inline bool full() const
    {
        return m_size == MaxSize;
    }

    inline void push_back(T const& what)
    {
        CPPA_REQUIRE(!full());
        m_data[m_size++] = what;
    }

    inline reference at(size_type pos)
    {
        CPPA_REQUIRE(pos < m_size);
        return m_data[pos];
    }

    inline const_reference at(size_type pos) const
    {
        CPPA_REQUIRE(pos < m_size);
        return m_data[pos];
    }

    inline reference operator[](size_type pos)
    {
        return at(pos);
    }

    inline const_reference operator[](size_type pos) const
    {
        return at(pos);
    }

    inline iterator begin()
    {
        return m_data;
    }

    inline const_iterator begin() const
    {
        return m_data;
    }

    inline const_iterator cbegin() const
    {
        return begin();
    }

    inline iterator end()
    {
        return begin() + m_size;
    }

    inline const_iterator end() const
    {
        return begin() + m_size;
    }

    inline const_iterator cend() const
    {
        return end();
    }

    inline reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    inline const_reverse_iterator rbegin() const
    {
        return reverse_iterator(end());
    }

    inline const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    inline reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    inline const_reverse_iterator rend() const
    {
        return reverse_iterator(begin());
    }

    inline const_reverse_iterator crend() const
    {
        return rend();
    }

    inline reference front()
    {
        CPPA_REQUIRE(!empty());
        return m_data[0];
    }

    inline const_reference front() const
    {
        CPPA_REQUIRE(!empty());
        return m_data[0];
    }

    inline reference back()
    {
        CPPA_REQUIRE(!empty());
        return m_data[m_size - 1];
    }

    inline const_reference back() const
    {
        CPPA_REQUIRE(!empty());
        return m_data[m_size - 1];
    }

    inline T* data()
    {
        return m_data;
    }

    inline T const* data() const
    {
        return m_data;
    }

    /**
     * @brief Inserts elements to specified position in the container.
     * @warning This member function is implemented for <tt>pos == end()</tt>
     *          only by now. The user has to guarantee that the size of the
     *          sequence [first, last) fits into the vector.
     */
    template<class InputIterator>
    inline void insert(iterator pos,
                       InputIterator first,
                       InputIterator last)
    {
        if (pos == end())
        {
            if (m_size + static_cast<size_type>(last - first) > MaxSize)
            {
                throw std::runtime_error("fixed_vector::insert: full");
            }
            for ( ; first != last; ++first)
            {
                push_back(*first);
            }
        }
        else
        {
            throw std::runtime_error("not implemented fixed_vector::insert");
        }
    }

};

} } // namespace cppa::util

#endif // FIXED_VECTOR_HPP
