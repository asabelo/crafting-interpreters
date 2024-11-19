
#include "debug.hpp"
#include "value.hpp"

void lox::disassemble_chunk(const chunk& chunk, const std::string& name)
{
    std::cout << std::format("== {} ==\n", name);

    for (chunk::idx_t offset = 0; offset < chunk.count();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

static lox::chunk::idx_t constant_instruction(const std::string& name, const lox::chunk& chunk, lox::chunk::idx_t offset)
{
    auto& constant_info = chunk.get(offset + 1);
    
    std::cout << std::format("{:16} {:4} '", name, constant_info.op);
    
    chunk.constants().get(constant_info.op).print();
    
    std::cout << "'\n";

    return offset + 2;
}

static lox::chunk::idx_t simple_instruction(const std::string& name, lox::chunk::idx_t offset)
{
    std::cout << name << '\n';

    return offset + 1;
}

lox::chunk::idx_t lox::disassemble_instruction(const chunk& chunk, chunk::idx_t offset)
{
    std::cout << std::format("{:0>4}", static_cast<int>(offset));
    
    const auto& op_info = chunk.get(offset);

    if (offset > 0 && op_info.line == chunk.get(offset - 1).line)
    {
        std::cout << "   | ";
    }
    else
    {
        std::cout << std::format("{:4} ", op_info.line);
    }

    auto instruction = op_info.op;

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

    case op_code::OP_EQUAL:
        return simple_instruction("OP_EQUAL", offset);
    
    case op_code::OP_GREATER:
        return simple_instruction("OP_GREATER", offset);

    case op_code::OP_LESS:
        return simple_instruction("OP_LESS", offset);

    case op_code::OP_ADD:
        return simple_instruction("OP_ADD", offset);

    case op_code::OP_SUBTRACT:
        return simple_instruction("OP_SUBTRACT", offset);

    case op_code::OP_MULTIPLY:
        return simple_instruction("OP_MULTIPLY", offset);

    case op_code::OP_DIVIDE:
        return simple_instruction("OP_DIVIDE", offset);

    case op_code::OP_NOT:
        return simple_instruction("OP_NOT", offset);

    case op_code::OP_NEGATE:
        return simple_instruction("OP_NEGATE", offset);

    case op_code::OP_RETURN:
        return simple_instruction("OP_RETURN", offset);

    default:
        std::cout << std::format("Unknown opcode {}\n", instruction);
        return offset + 1;
    }
}
