
#include "chunk.hpp"

int main(int argc, char* argv[])
{
    auto c = lox::chunk{};

    c.write(1);
    c.write(2);
    c.write(3);

    return 0;
}
