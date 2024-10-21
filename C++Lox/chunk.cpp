
#include "chunk.hpp"

void lox::chunk::write(uint8_t byte)
{
    if (capacity < count + 1)
    {
        auto old_capacity = capacity;
        auto new_capacity = capacity = capacity < 8 ? 8 : capacity * 1.5;

        code = reallocate(std::move(code), old_capacity, new_capacity);
    }

    code[count++] = byte;
}
