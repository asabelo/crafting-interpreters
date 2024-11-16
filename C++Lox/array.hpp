
#pragma once

#include <concepts>

#include "collection.hpp"
#include "memory.hpp"

namespace lox
{
    template <typename TElement, typename TIndex = std::size_t, std::unsigned_integral TCapacity = std::size_t>
    class array : public collection<TElement, TIndex, TCapacity>
    {
    public:

        using cap_t  = TCapacity;
        using idx_t  = TIndex;
        using elem_t = TElement;

    protected:

        cap_t m_count = 0;

        cap_t m_capacity = 0;

        elem_t* m_elements = nullptr;

    public:

        array() = default;

        ~array()
        {
            if (m_elements) free_array(m_elements, m_capacity);
        }

        ///
        /// Copy constructor.
        ///
        array(const array& other)
        {
            m_count    = other.m_count;
            m_capacity = other.m_capacity;
            m_elements = grow_array(m_elements, 0, other.m_capacity);

            std::copy(other.m_elements, other.m_elements + other.m_count, m_elements);
        }

        ///
        /// Copy assignment operator.
        ///
        array& operator=(array other)
        {
            std::swap(m_count,    other.m_count);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_elements, other.m_elements);

            return *this;
        }

        ///
        /// Move constructor.
        ///
        array(array&& other)
        {
            m_count    = other.m_count;
            m_capacity = other.m_capacity;
            m_elements = other.m_elements;

            other.m_count = 0;
            other.m_capacity = 0;
            other.m_elements = nullptr;
        }

        ///
        /// Move assignment operator.
        ///
        array& operator=(array&& other)
        {
            if (&other != this)
            {
                if (m_elements) free_array(m_elements, m_capacity);

                m_count    = other.m_count;
                m_capacity = other.m_capacity;
                m_elements = other.m_elements;

                other.m_count    = 0;
                other.m_capacity = 0;
                other.m_elements = nullptr;
            }

            return *this;
        }

        ///
        /// Returns the amount of elements the array currently contains.
        ///
        cap_t count() const override
        {
            return m_count;
        }

        ///
        /// Returns the maximum amount of elements the array can contain before a reallocation.
        ///
        cap_t capacity() const
        {
            return m_capacity;
        }

        ///
        /// Retrieves a reference to an element at an index.
        ///
        elem_t& get(idx_t index) override
        {
            return m_elements[index]; 
        }

        ///
        /// Retrieves a const reference to an element at an index. 
        ///
        const elem_t& get(idx_t index) const override
        { 
            return m_elements[index];
        }

        ///
        /// Adds a new element to the array and returns its index.
        ///
        idx_t add(elem_t element) override
        {
            if (m_capacity < m_count + 1)
            {
                auto old_capacity = m_capacity;
                auto new_capacity = m_capacity = grow_capacity(m_capacity);

                m_elements = grow_array(m_elements, old_capacity, new_capacity);
            }

            m_elements[m_count++] = element;

            return m_count - 1;
        }

        ///
        /// Sets the array's count to 0 but does not shrink its capacity.
        ///
        void reset()
        {
            m_count = 0;
        }
    };
}
