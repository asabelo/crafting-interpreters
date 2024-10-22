
#pragma once

#include "memory.hpp"

namespace lox
{
    template <typename T>
    class array
    {
    protected:

        std::size_t count = 0;

        std::size_t capacity = 0;

        T* code = nullptr;

    public:

        std::size_t get_count() const { return count; }

        std::size_t get_capacity() const { return capacity; }

        T const* get_code() const { return code; }

        ~array()
        {
            if (code) free_array(code, capacity);
        }

        void write(T element)
        {
            if (capacity < count + 1)
            {
                auto old_capacity = capacity;
                auto new_capacity = grow_capacity(capacity);

                code = grow_array(code, old_capacity, new_capacity);
            }

            code[count++] = element;
        }
    };
}
