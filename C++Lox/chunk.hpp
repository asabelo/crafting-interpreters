
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
        value_array m_constants = {};

    public:

        value_array& constants() { return m_constants; }

        const value_array& constants() const { return m_constants; }
    };
}
