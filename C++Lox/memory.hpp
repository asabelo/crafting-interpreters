
#pragma once

#include "common.hpp"

namespace lox
{
    static constexpr std::size_t grow_capacity(std::size_t old_capacity, double grow_factor = 1.5)
    {
        return static_cast<std::size_t>
        (
            old_capacity < 8 ? 8 : old_capacity * grow_factor
        );
    }

    template <typename T>
    static inline void grow_array(std::unique_ptr<T[]>& old_ptr, std::size_t old_count, std::size_t new_count)
    {
        auto new_ptr = std::make_unique<T[]>(new_count);

        for (std::size_t i = 0, max_i = std::min(old_count, new_count); i < max_i; ++i)
        {
            std::swap(old_ptr[i], new_ptr[i]);
        }

        std::swap(old_ptr, new_ptr);
    }

    template <typename T>
    static inline std::unique_ptr<T[]> allocate_array(std::size_t count)
    {
        return std::make_unique<T[]>(count);
    }

    template <typename T, typename... U>
    static inline std::shared_ptr<T> allocate_shared(U&&... args)
    {
        return std::make_shared<T>(std::forward<U>(args)...);
    }

    template <typename T, typename... U>
    static inline std::unique_ptr<T> allocate_unique(U&&... args)
    {
        return std::make_unique<T>(std::forward<U>(args)...);
    }
}
