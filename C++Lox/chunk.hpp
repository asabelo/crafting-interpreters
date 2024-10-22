
#pragma once

#include "array.hpp"
#include "common.hpp"
#include "value.hpp"

namespace lox
{
    // Bytecode instruction operation code
    enum op_code : uint8_t
    {
        OP_RETURN,
        OP_CONSTANT
    };

    class chunk : public array<uint8_t>
    {
        using line_array = array<int>;

        line_array m_lines = {};

        value_array m_constants = {};

    public:
        line_array& lines() { return m_lines; }
        const line_array& lines() const { return m_lines; }

        value_array& constants() { return m_constants; }
        const value_array& constants() const { return m_constants; }

        uint8_t add(uint8_t element, int line)
        {
            auto elem_ix = array::add(element);
            auto line_ix = m_lines.add(line);

            if (elem_ix != line_ix) std::exit(1);

            return elem_ix;
        }
    };
}
