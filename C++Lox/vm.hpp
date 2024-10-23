
#pragma once

#include "chunk.hpp"
#include "common.hpp"

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

        //inline const chunk::elem_t read_byte()
        //{
        //    return ;
        //}

        //inline const value_array::elem_t read_constant()
        //{
        //    return chunk.constants().get(read_byte());
        //}

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
                switch (read_byte())
                {
                case op_code::OP_CONSTANT:
                    print_value(read_constant());
                    std::printf("\n");
                    break;

                case op_code::OP_RETURN:
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
