
#include "debug.hpp"
#include "value.hpp"

void lox::disassemble_chunk(const chunk& chunk, const std::string& name)
{
    std::cout << std::format("== {} ==\n", name);

    for (int offset = 0; offset < chunk.count();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

static lox::chunk::idx_t constant_instruction(const std::string& name, const lox::chunk& chunk, lox::chunk::idx_t offset)
{
    auto constant = chunk.get(offset + 1);
    
    std::cout << std::format("{:16} {:4} '", name, constant);
    
    lox::print_value(chunk.constants().get(constant));
    
    std::cout << "'\n";

    return offset + 2;
}

static lox::chunk::idx_t simple_instruction(const std::string& name, lox::chunk::idx_t offset)
{
    std::cout << name << '\n';

    return offset + 1;
}

int lox::disassemble_instruction(const chunk& chunk, chunk::idx_t offset)
{
    std::cout << std::format("{:0>4}", static_cast<int>(offset));

    auto& lines = chunk.lines();

    if (offset > 0 && lines.get(offset) == lines.get(offset - 1))
    {
        std::cout << "   | ";
    }
    else
    {
        std::cout << std::format("{:4} ", lines.get(offset));
    }

    auto instruction = chunk.get(offset);

    switch (instruction)
    {
    case op_code::OP_CONSTANT:
        return constant_instruction("OP_CONSTANT", chunk, offset);

    case op_code::OP_NIL:
        return simple_instruction("OP_NIL", offset);

    case op_code::OP_TRUE:
        return simple_instruction("OP_TRUE", offset);

    case op_code::OP_FALSE:
        return simple_instruction("OP_FALSE", offset);

    case op_code::OP_ADD:
        return simple_instruction("OP_ADD", offset);

    case op_code::OP_SUBTRACT:
        return simple_instruction("OP_SUBTRACT", offset);

    case op_code::OP_MULTIPLY:
        return simple_instruction("OP_MULTIPLY", offset);

    case op_code::OP_DIVIDE:
        return simple_instruction("OP_DIVIDE", offset);

    case op_code::OP_NEGATE:
        return simple_instruction("OP_NEGATE", offset);

    case op_code::OP_RETURN:
        return simple_instruction("OP_RETURN", offset);

    default:
        std::printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
