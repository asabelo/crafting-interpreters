
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "memory.hpp"

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

        std::unique_ptr<uint8_t[]> code = nullptr;

    public:

        void write(uint8_t byte);
    };
}
