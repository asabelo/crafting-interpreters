
#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, char* argv[])
{
    auto c = lox::chunk{};

    auto constant = c.constants().add(69);
    c.add(lox::op_code::OP_CONSTANT);
    c.add(constant);
    c.add(lox::op_code::OP_RETURN);

    lox::disassemble_chunk(c, "test chunk");

    return 0;
}
