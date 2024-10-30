
#pragma once

#include "array.hpp"
#include "common.hpp"
#include "value.hpp"

namespace lox
{
    // Bytecode instruction operation code
    enum op_code : uint8_t
    {
        OP_CONSTANT,
        OP_NIL,
        OP_TRUE,
        OP_FALSE,
        OP_EQUAL,
        OP_GREATER,
        OP_LESS,
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_NOT,
        OP_NEGATE,
        OP_RETURN
    };

    class chunk : public array<uint8_t>
    {
        using line_array = array<int>;

        line_array m_lines = {};

        value_array m_constants = {};

    public:
              line_array& lines()       { return m_lines; }
        const line_array& lines() const { return m_lines; }

              value_array& constants()       { return m_constants; }
        const value_array& constants() const { return m_constants; }

        uint8_t add(uint8_t element, int line)
        {
            m_lines.add(line);
            return array::add(element);
        }
    };
}
