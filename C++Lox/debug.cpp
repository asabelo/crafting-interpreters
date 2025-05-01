
#include "debug.hpp"
#include "value.hpp"

void lox::disassemble_chunk(const chunk& chunk, const std::string& name)
{
    std::cout << std::format("== {} ==\n", name);

    for (chunk::size_type offset = 0; offset < chunk.size();)
    {
        offset = disassemble_instruction(chunk, offset);
    }
}

static lox::chunk::size_type constant_instruction(const std::string& name, const lox::chunk& chunk, lox::chunk::size_type offset)
{
    auto& constant_info = chunk.at(offset + 1);
    
    std::cout << std::format("{:16} {:4} '", name, constant_info.op);
    
    chunk.constants().at(constant_info.op).print();
    
    std::cout << "'\n";

    return offset + 2;
}

static lox::chunk::size_type simple_instruction(const std::string& name, lox::chunk::size_type offset)
{
    std::cout << name << '\n';

    return offset + 1;
}

lox::chunk::size_type lox::disassemble_instruction(const chunk& chunk, chunk::size_type offset)
{
    std::cout << std::format("{:0>4}", static_cast<int>(offset));
    
    const auto& op_info = chunk.at(offset);

    if (offset > 0 && op_info.line == chunk.at(offset - 1).line)
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

    case op_code::OP_POP:
        return simple_instruction("OP_POP", offset);

    case op_code::OP_GET_GLOBAL:
        return constant_instruction("OP_GET_GLOBAL", chunk, offset);

    case op_code::OP_DEFINE_GLOBAL:
        return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);

    case op_code::OP_SET_GLOBAL:
        return constant_instruction("OP_SET_GLOBAL", chunk, offset);

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

    case op_code::OP_PRINT:
        return simple_instruction("OP_PRINT", offset);

    case op_code::OP_RETURN:
        return simple_instruction("OP_RETURN", offset);

    default:
        std::cout << std::format("Unknown opcode {}\n", instruction);
        return offset + 1;
    }
}
