
#include "chunk.hpp"
#include "common.hpp"
#include "debug.hpp"
#include "vm.hpp"

int main(int argc, char* argv[])
{
    auto vm = lox::vm{};

    auto c = lox::chunk{};
    c.add(lox::op_code::OP_CONSTANT, 123);
    c.add(c.constants().add(1.2), 123);

    c.add(lox::op_code::OP_CONSTANT, 123);
    c.add(c.constants().add(3.4), 123);

    c.add(lox::op_code::OP_ADD, 123);

    c.add(lox::op_code::OP_CONSTANT, 123);
    c.add(c.constants().add(5.6), 123);

    c.add(lox::op_code::OP_DIVIDE, 123);

    c.add(lox::op_code::OP_NEGATE, 123);

    c.add(lox::op_code::OP_RETURN, 123);

    vm.interpret(std::move(c));

    return 0;
}
