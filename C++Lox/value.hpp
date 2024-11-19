
#pragma once

#include <variant>

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

    class value
    {
        value_type m_type;
        std::variant<std::monostate, bool, double, std::shared_ptr<obj>> m_inner;

    public:

        value();
        value(bool);
        value(double);
        value(std::shared_ptr<obj>);

        static value nil();
        static value from(bool value);
        static value from(double value);
        static value from(std::shared_ptr<obj> value);

        bool is_nil() const;
        bool is_boolean() const;
        bool is_number() const;
        bool is_object() const;
        bool is_string() const;
        bool is_falsey() const;

        bool as_boolean() const;
        double as_number() const;
        std::shared_ptr<obj> as_object() const;

        bool equals(const value& other) const;

        void print() const;
    };

    using value_array = array<value, uint8_t>;
}
