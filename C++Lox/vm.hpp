
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

        void concatenate();

        void runtime_error(const std::string_view format, const auto&&... params);

    public:

        static obj* objects;

        vm(lox::chunk& chunk);

        ~vm();

        interpret_result interpret(const std::string_view source);
    };
}
