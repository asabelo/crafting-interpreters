
#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, char* argv[])
{
    auto c = lox::chunk{};

    c.write(lox::op_code::OP_RETURN);

    lox::disassemble_chunk(c, "test chunk");

    return 0;
}
