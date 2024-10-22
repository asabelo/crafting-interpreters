
#pragma once

#include "array.hpp"
#include "common.hpp"

namespace lox
{
    using value = double;

    using value_array = array<value, uint8_t>;

    void print_value(value value);
}
