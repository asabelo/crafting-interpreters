
#pragma once

#include "chunk.hpp"
#include "common.hpp"
#include "stack.hpp"

namespace lox
{
    enum class interpret_result
    {
        INTERPRET_OK,
        INTERPRET_COMPILE_ERROR,
        INTERPRET_RUNTIME_ERROR
    };

    class vm
    {
        chunk chunk = {};

        chunk::idx_t ip = {};

        stack<value> stack = {};

        interpret_result run();

    public:

        interpret_result interpret(const std::string_view source);
    };
}
