
#pragma once

#include <vector>

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

    class chunk : public std::vector<op_info>
    {
        std::vector<value> m_constants = {};

    public:

        ///
        /// Returns a reference to the array of constants of this chunk.
        ///
        std::vector<value>& constants()
        {
            return m_constants;
        }

        ///
        /// Returns a const reference to the array of constants of this chunk.
        ///
        const std::vector<value>& constants() const
        {
            return m_constants;
        }
    };
}
