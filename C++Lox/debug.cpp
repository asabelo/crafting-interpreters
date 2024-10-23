
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

static lox::chunk::idx_t constant_instruction(const char* name, const lox::chunk& chunk, lox::chunk::idx_t offset)
{
    auto constant = chunk.get(offset + 1);
    
    std::printf("%-16s %4d '", name, constant);
    
    lox::print_value(chunk.constants().get(constant));
    
    std::printf("'\n");

    return offset + 2;
}

static lox::chunk::idx_t simple_instruction(const char* name, lox::chunk::idx_t offset)
{
    std::printf("%s\n", name);

    return offset + 1;
}

int lox::disassemble_instruction(const chunk& chunk, chunk::idx_t offset)
{
    std::printf("%04d ", static_cast<int>(offset));

    auto& lines = chunk.lines();
    if (offset > 0 && lines.get(offset) == lines.get(offset - 1))
    {
        printf("   | ");
    }
    else
    {
        printf("%4d ", lines.get(offset));
    }

    auto instruction = chunk.get(offset);

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
