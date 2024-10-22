
#include "chunk.hpp"
#include "memory.hpp"

void lox::chunk::init()
{
    count = 0;
    capacity = 0;
    code = nullptr;
}

void lox::chunk::write(uint8_t byte)
{
    if (capacity < count + 1)
    {
        auto old_capacity = capacity;
        auto new_capacity = grow_capacity(capacity);

        code = grow_array(code, old_capacity, new_capacity);
    }

    code[count++] = byte;
}
