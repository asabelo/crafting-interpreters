
#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, char* argv[])
{
    auto c = lox::chunk{};

    auto constant = c.constants().add(1.2);
    c.add(lox::op_code::OP_CONSTANT, 123);
    c.add(constant, 123);
    c.add(lox::op_code::OP_RETURN, 123);

    lox::disassemble_chunk(c, "test chunk");

    return 0;
}
