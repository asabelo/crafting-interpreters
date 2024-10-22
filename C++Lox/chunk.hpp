
#pragma once

#include "array.hpp"
#include "common.hpp"

namespace lox
{
    // Bytecode instruction operation code
    enum class op_code : uint8_t
    {
        OP_RETURN
    };

    using chunk = array<op_code>;
}
