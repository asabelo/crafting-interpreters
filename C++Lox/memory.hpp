
#pragma once

#include "common.hpp"

namespace lox
{
    static constexpr std::size_t grow_capacity(std::size_t old_capacity, double grow_factor = 1.5)
    {
        return old_capacity < 8 ? 8 : old_capacity * grow_factor;
    }

    static void* reallocate(void* pointer, std::size_t old_size, std::size_t new_size)
    {
        if (new_size == 0)
        {
            std::free(pointer);

            return nullptr;
        }

        auto result = std::realloc(pointer, new_size);

        if (!result) std::exit(1);

        return result;
    }

    template <typename T>
    static T* grow_array(T* pointer, std::size_t old_count, std::size_t new_count)
    {
        return (T*)reallocate(pointer, sizeof(T) * old_count, sizeof(T) * new_count);
    }

    template <typename T>
    static T* free_array(T* pointer, std::size_t old_count)
    {
        return (T*)reallocate(pointer, sizeof(T) * old_count, 0);
    }
}
