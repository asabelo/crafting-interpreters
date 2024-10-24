
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

        interpret_result run()
        {
            const auto read_byte = [this]()
            {
                return chunk.get(ip++);
            };

            const auto read_constant = [this, &read_byte]()
            {
                return chunk.constants().get(read_byte());
            };

            while (true)
            {
#ifdef _DEBUG
                std::printf("          ");
                for (lox::stack<value>::idx_t i = 0; i < stack.count(); ++i)
                {
                    std::printf("[ ");
                    print_value(stack.get(i));
                    std::printf(" ]");
                }
                std::printf("\n");

                disassemble_instruction(chunk, ip);
#endif // DEBUG

                switch (read_byte())
                {
                case op_code::OP_CONSTANT:
                    stack.push(read_constant());
                    break;

                case op_code::OP_RETURN:
                    print_value(stack.pop());
                    std::printf("\n");
                    return interpret_result::INTERPRET_OK;
                }
            }
        }

    public:

        interpret_result interpret(lox::chunk&& chunk)
        {
            this->chunk = std::move(chunk);

            return run();
        }
    };
}
