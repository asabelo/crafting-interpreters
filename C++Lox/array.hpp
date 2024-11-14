
#pragma once

#include <concepts>

#include "memory.hpp"

namespace lox
{
    template <typename TElement, std::unsigned_integral TIndex = std::size_t>
    class array
    {
    public:

        using idx_t = TIndex;
        using elem_t = TElement;

    protected:

        idx_t m_count = 0;

        idx_t m_capacity = 0;

        elem_t* m_elements = nullptr;

    public:

        array() = default;

        ~array()
        {
            if (m_elements) free_array(m_elements, m_capacity);
        }

        array(const array& other)
        {
            m_count    = other.m_count;
            m_capacity = other.m_capacity;
            m_elements = grow_array(nullptr, 0, other.m_capacity);

            std::copy(other.m_elements, other.m_elements + other.m_count, m_elements);
        }

        array& operator=(array other)
        {
            std::swap(m_count,    other.m_count);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_elements, other.m_elements);

            return *this;
        }

        array(array&& other)
        {
            m_count    = other.m_count;
            m_capacity = other.m_capacity;
            m_elements = other.m_elements;

            other.m_count = 0;
            other.m_capacity = 0;
            other.m_elements = nullptr;

            return *this;
        }

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

        idx_t capacity() const { return m_capacity; }

              elem_t& get(idx_t index)       { return m_elements[index]; }
        const elem_t& get(idx_t index) const { return m_elements[index]; }

        idx_t add(elem_t element)
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

        void reset()
        {
            m_count = 0;
        }
    };
}
