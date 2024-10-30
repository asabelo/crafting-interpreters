
#pragma once

#include "array.hpp"
#include "common.hpp"

namespace lox
{
    enum class value_type
    {
        BOOL,
        NIL,
        NUMBER
    };

    struct value
    {
        value_type type;
        union
        {
            bool boolean;
            double number;
        } as;

        static value nil();

        static value from(bool value);

        static value from(double value);
        
        bool is_nil() const;

        bool is_boolean() const;
        
        bool is_number() const;
    };

    using value_array = array<value, uint8_t>;

    void print_value(value value);
}
