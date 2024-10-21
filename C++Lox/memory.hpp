
#pragma once

#include <memory>

namespace lox
{
    template<typename T>
    std::unique_ptr<T[]> reallocate(std::unique_ptr<T[]>&& pointer, size_t old_size, size_t new_size)
    {
        if (new_size == 0) return {};

        std::unique_ptr<T[]> new_pointer;

        try
        {
            new_pointer = std::make_unique<T[]>(new_size);
            
            if (old_size > 0) std::copy_n(pointer.get(), std::min(old_size, new_size), new_pointer.get());
        }
        catch (std::bad_alloc)
        {
            std::exit(1);
        }

        return new_pointer;
    }
}
