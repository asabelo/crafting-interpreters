
#pragma once

#include "common.hpp"

namespace lox
{
    template <typename TElement, typename TIndex = std::size_t, std::unsigned_integral TCapacity = std::size_t>
    class collection
    {
    public:
        ///
        /// Returns the amount of elements contained in the collection.
        ///
        virtual TCapacity count() const = 0;

        ///
        /// Retrieves a reference to an element at an index.
        ///
        virtual TElement& get(TIndex index) = 0;

        ///
        /// Retrieves a const reference to an element at an index. 
        ///
        virtual const TElement& get(TIndex index) const = 0;

        ///
        /// Adds a new element to the collection and returns its index.
        ///
        virtual TIndex add(TElement element) = 0;
    };
}
