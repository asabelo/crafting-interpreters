
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

        void push(elem_t element) { this->add(element); }

        elem_t pop() { return this->m_elements[--this->m_count]; }

              elem_t& peek()       { return this->m_elements[this->m_count - 1]; }
        const elem_t& peek() const { return this->m_elements[this->m_count - 1]; }
    };
}
