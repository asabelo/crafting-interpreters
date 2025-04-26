
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

        void print() const;

        template <typename T> friend struct std::equal_to;
    };

    using value_array = array<value, uint8_t>;
}

namespace std
{
    template <>
    struct std::equal_to<lox::value>
    {
        bool operator()(const lox::value& lhs, const lox::value& rhs) const
        {
            lox::value_type type;
            if ((type = lhs.m_type) != rhs.m_type) return false;

            switch (type)
            {
            case lox::value_type::BOOL:   return std::equal_to{}(lhs.as_boolean(), rhs.as_boolean());
            case lox::value_type::NIL:    return true;
            case lox::value_type::NUMBER: return std::equal_to{}(lhs.as_number(), rhs.as_number());
            case lox::value_type::OBJECT: return std::equal_to<lox::obj>{}(*lhs.as_object(), *rhs.as_object());
            }

            return false;
        }
    };
}
