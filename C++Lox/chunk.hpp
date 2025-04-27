
#pragma once

#include "array.hpp"
#include "collection.hpp"
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
        OP_POP,
        OP_GET_GLOBAL,
        OP_DEFINE_GLOBAL,
        OP_SET_GLOBAL,
        OP_EQUAL,
        OP_GREATER,
        OP_LESS,
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_NOT,
        OP_NEGATE,
        OP_PRINT,
        OP_RETURN
    };

    struct op_info
    {
        uint8_t op;
        int line;
    };

    class chunk : public array<op_info>
    {
        value_array m_constants = {};

    public:

        ///
        /// Returns a reference to the array of constants of this chunk.
        ///
        value_array& constants()
        {
            return m_constants;
        }

        ///
        /// Returns a const reference to the array of constants of this chunk.
        ///
        const value_array& constants() const
        {
            return m_constants;
        }
    };
}
