
#pragma once

#include "array.hpp"
#include "common.hpp"

namespace lox
{
    template <typename TElement>
    class stack : public array<TElement>
    {
    public:

        using elem_t = stack::array::elem_t;
        using idx_t = stack::array::idx_t;

        void push(elem_t element) { this->add(element); }

        elem_t pop() { return this->m_elements[--this->m_count]; }

              elem_t& peek(idx_t depth = 0)       { return this->m_elements[this->m_count - depth - 1]; }
        const elem_t& peek(idx_t depth = 0) const { return this->m_elements[this->m_count - depth - 1]; }
    };
}
