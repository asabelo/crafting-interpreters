
#include "common.hpp"
#include "chunk.hpp"
#include "debug.hpp"

int main(int argc, char* argv[])
{
    auto c = lox::chunk{};

    //c.add(lox::op_code::OP_CONSTANT, 123);
    //c.add(c.constants().add(1.2), 123);

    c.add(lox::op_code::OP_RETURN, 123);
    c.add(lox::op_code::OP_RETURN, 123);

    c.add(lox::op_code::OP_RETURN, 123);

    c.add(lox::op_code::OP_CONSTANT, 124);
    c.add(c.constants().add(45), 124);

    c.add(lox::op_code::OP_RETURN, 124);
    c.add(lox::op_code::OP_RETURN, 125);
    c.add(lox::op_code::OP_RETURN, 126);
    c.add(lox::op_code::OP_RETURN, 127);
    c.add(lox::op_code::OP_RETURN, 127);
    c.add(lox::op_code::OP_RETURN, 127);
    c.add(lox::op_code::OP_RETURN, 127);



    lox::disassemble_chunk(c, "test chunk");

    return 0;
}
