
#include "debug.hpp"
#include "value.hpp"

void lox::disassemble_chunk(const chunk& chunk, const char* name)
{
    std::printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk.count();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

static int constant_instruction(const char* name, const lox::chunk& chunk, int offset)
{
    auto constant = chunk.get()[offset + 1];
    
    std::printf("%-16s %4d '", name, constant);
    
    lox::print_value(chunk.constants().get()[constant]);
    
    std::printf("'\n");

    return offset + 2;
}

static int simple_instruction(const char* name, int offset)
{
    std::printf("%s\n", name);

    return offset + 1;
}

int lox::disassemble_instruction(const chunk& chunk, int offset)
{
    std::printf("%04d ", offset);

    auto lines = chunk.lines().get();
    if (offset > 0 && lines[offset] == lines[offset - 1])
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", lines[offset]);
    }

    auto instruction = chunk.get()[offset];

    switch (instruction)
    {
    case op_code::OP_CONSTANT:
        return constant_instruction("OP_CONSTANT", chunk, offset);

    case op_code::OP_RETURN:
        return simple_instruction("OP_RETURN", offset);

    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
