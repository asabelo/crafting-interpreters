
#pragma once

#include "array.hpp"
#include "common.hpp"
#include "object.hpp"

namespace lox
{
    enum class value_type
    {
        BOOL,
        NIL,
        NUMBER,
        OBJECT
    };

    struct value
    {
        value_type type;
        union
        {
            bool boolean;
            double number;
            obj* object;
        } as;

        static value nil();

        static value from(bool value);

        static value from(double value);

        static value from(obj* value);
        
        bool is_nil() const;

        bool is_boolean() const;
        
        bool is_number() const;

        bool is_object() const;

        bool is_string() const;

        bool is_falsey() const;

        bool equals(const value& other) const;
    };

    using value_array = array<value, uint8_t>;

    void print_value(value value);
}
