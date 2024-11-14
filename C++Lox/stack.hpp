
#pragma once

#include "array.hpp"
#include "common.hpp"

namespace lox
{
    template <typename TElement, std::unsigned_integral TCapacity = std::size_t>
    class stack : public array<TElement, std::size_t, TCapacity>
    {
    public:

        using elem_t = stack::array::elem_t;
        using idx_t  = stack::array::idx_t;

        ///
        /// Adds an element on top of the stack.
        ///
        void push(elem_t element)
        {
            this->add(element);
        }

        ///
        /// Removes an element from the top of the stack and returns it.
        ///
        elem_t pop()
        {
            return this->m_elements[--this->m_count];
        }

        ///
        /// Returns a reference to an element at a depth starting from the top.
        ///
        elem_t& peek(idx_t depth = 0)
        {
            return this->m_elements[this->m_count - depth - 1];
        }

        ///
        /// Returns a const reference to an element at a depth starting from the top.
        ///
        const elem_t& peek(idx_t depth = 0) const
        {
            return this->m_elements[this->m_count - depth - 1];
        }
    };
}
