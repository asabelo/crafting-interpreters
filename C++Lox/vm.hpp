
#pragma once

#include <vector>

#include "chunk.hpp"
#include "common.hpp"

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

        chunk::size_type m_ip;

        std::vector<value> m_stack;

        std::unordered_map<std::string_view, std::shared_ptr<obj_string>> m_strings;

        std::unordered_map<std::shared_ptr<obj_string>, value> m_globals;

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
