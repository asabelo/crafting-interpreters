
#include <cstdio>

#include "debug.hpp"

void lox::disassemble_chunk(const chunk& chunk, const char* name)
{
    std::printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk.get_count();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int simple_instruction(const char* name, int offset)
{
    printf("%s\n", name);

    return offset + 1;
}

int lox::disassemble_instruction(const chunk& chunk, int offset)
{
    std::printf("%04d ", offset);

    auto instruction = chunk.get_code()[offset];

    switch (instruction)
    {
        case op_code::OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
