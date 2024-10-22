
#pragma once

#include "common.hpp"

namespace lox
{
    // Bytecode instruction operation code
    enum class op_code
    {
        OP_RETURN
    };

    struct chunk
    {
        std::size_t count = 0;

        std::size_t capacity = 0;

        uint8_t* code = nullptr;

    public:

        void init();

        void write(uint8_t byte);
    };
}
