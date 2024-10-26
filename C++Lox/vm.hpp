
#pragma once

#include <functional>

#include "chunk.hpp"
#include "common.hpp"
#include "compiler.hpp"
#include "debug.hpp"
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

            const auto binary_op = [this](std::function<value(value, value)> op)
            {
                const auto b = stack.pop();
                const auto a = stack.pop();
                return stack.push(op(a, b));
            };

            while (true)
            {
#ifdef _DEBUG
                std::cout << "          ";

                for (lox::stack<value>::idx_t i = 0; i < stack.count(); ++i)
                {
                    std::cout << "[ ";

                    print_value(stack.get(i));

                    std::cout << " ]";
                }

                std::cout << '\n';

                disassemble_instruction(chunk, ip);
#endif // DEBUG

                switch (read_byte())
                {
                case op_code::OP_CONSTANT:
                    stack.push(read_constant());
                    break;

                case op_code::OP_ADD:
                    binary_op(std::plus{});
                    break;

                case op_code::OP_SUBTRACT:
                    binary_op(std::minus{});
                    break;

                case op_code::OP_MULTIPLY:
                    binary_op(std::multiplies{});
                    break;

                case op_code::OP_DIVIDE:
                    binary_op(std::divides{});
                    break;

                case op_code::OP_NEGATE:
                    stack.push(-stack.pop());
                    break;

                case op_code::OP_RETURN:
                    print_value(stack.pop());
                    std::cout << '\n';
                    return interpret_result::INTERPRET_OK;
                }
            }
        }

    public:

        interpret_result interpret(const std::string_view source)
        {
            auto scanner = lox::scanner{ source };

            compile(scanner);

            return interpret_result::INTERPRET_OK;
        }
    };
}
