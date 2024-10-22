
#pragma once

#include <concepts>

#include "memory.hpp"

namespace lox
{
    template <typename TElement, std::unsigned_integral TIndex = std::size_t>
    class array
    {
    protected:

        TIndex m_count = 0;

        TIndex m_capacity = 0;

        TElement* m_elements = nullptr;

    public:

        array() = default;

        ~array()
        {
            if (m_elements) free_array(m_elements, m_capacity);
        }

        array(array& other) = delete;

        array& operator=(array other) = delete;

        TIndex count() const { return m_count; }

        TIndex capacity() const { return m_capacity; }

        TElement const* get() const { return m_elements; }

        TIndex add(TElement element)
        {
            if (m_capacity < m_count + 1)
            {
                auto old_capacity = m_capacity;
                auto new_capacity = grow_capacity(m_capacity);

                m_elements = grow_array(m_elements, old_capacity, new_capacity);
            }

            m_elements[m_count++] = element;

            return m_count - 1;
        }
    };
}
