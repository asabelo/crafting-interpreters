
#pragma once

#include "chunk.hpp"
#include "common.hpp"
#include "stack.hpp"

namespace lox
{
    enum class interpret_result : int
    {
        OK,
        COMPILE_ERROR,
        RUNTIME_ERROR
    };

    class vm
    {
        chunk& m_chunk;

        chunk::idx_t m_ip;

        stack<value> m_stack;

        interpret_result run();

        void runtime_error(const std::string_view format, const auto&&... params);

    public:

        vm(lox::chunk& chunk);

        interpret_result interpret(const std::string_view source);
    };
}
