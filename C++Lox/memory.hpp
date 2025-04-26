
#pragma once

#include "common.hpp"

namespace lox
{
    template <std::unsigned_integral TCapacity = std::size_t>
    static constexpr TCapacity grow_capacity(TCapacity old_capacity, double grow_factor = 1.5)
    {
        return static_cast<TCapacity>
        (
            old_capacity < 8 ? 8 : old_capacity * grow_factor
        );
    }

    ///
    /// Resizes an array from a capacity of 'old_count' to 'new_count'.
    ///
    template
    <
        typename TArrayElement,
        std::unsigned_integral TCapacity = std::size_t
    >
    static inline void resize_array(std::unique_ptr<TArrayElement[]>& old_ptr, TCapacity old_count, TCapacity new_count)
    {
        auto new_ptr = std::make_unique<TArrayElement[]>(new_count);

        for (TCapacity i = 0, max_i = std::min(old_count, new_count); i < max_i; ++i)
        {
            std::swap(old_ptr[i], new_ptr[i]);
        }

        std::swap(old_ptr, new_ptr);
    }

    ///
    /// Creates a unique pointer to a new array with a capacity of 'count'.
    /// (Wrapper for make_unique.)
    ///
    template
    <
        typename TArrayElement,
        std::unsigned_integral TCapacity = std::size_t
    >
    static inline std::unique_ptr<TArrayElement[]> allocate_array(TCapacity count)
    {
        return std::make_unique<TArrayElement[]>(count);
    }

    ///
    /// Creates a shared pointer.
    /// (Wrapper for make_shared.)
    ///
    template
    <
        typename TElement,
        typename... TArgs
    >
    static inline std::shared_ptr<TElement> allocate_shared(TArgs&&... args)
    {
        return std::make_shared<TElement>(std::forward<TArgs>(args)...);
    }

    ///
    /// Creates a unique pointer.
    /// (Wrapper for make_unique.)
    ///
    template
    <
        typename TElement,
        typename... TArgs
    >
    static inline std::unique_ptr<TElement> allocate_unique(TArgs&&... args)
    {
        return std::make_unique<TElement>(std::forward<TArgs>(args)...);
    }
}
