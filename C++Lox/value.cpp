
#include "value.hpp"

lox::value lox::value::nil()
{
    return
    {
        .type{ value_type::NIL },
        .as{ .number{ 0 } }
    };
}

lox::value lox::value::from(bool value)
{
    return
    {
        .type{ value_type::BOOL },
        .as{ .boolean{ value } }
    };
}

lox::value lox::value::from(double value)
{
    return
    {
        .type{ value_type::NUMBER },
        .as{ .number{ value } }
    };
}

lox::value lox::value::from(obj* value)
{
    return
    {
        .type{ value_type::OBJECT },
        .as{ .object{ value } }
    };
}

bool lox::value::is_nil() const
{
    return this->type == value_type::NIL;
}

bool lox::value::is_boolean() const
{
    return this->type == value_type::BOOL;
}

bool lox::value::is_number() const
{
    return this->type == value_type::NUMBER;
}

bool lox::value::is_object() const
{
    return this->type == value_type::OBJECT;
}

bool lox::value::is_string() const
{
    return is_object() && this->as.object->type == obj_type::STRING;
}

bool lox::value::is_falsey() const
{
    return is_nil() || (is_boolean() && !as.boolean);
}

bool lox::value::equals(const value other) const
{
    if (this->type != other.type) return false;

    switch (type)
    {
    case value_type::BOOL:   return this->as.boolean == other.as.boolean;
    case value_type::NIL:    return true;
    case value_type::NUMBER: return this->as.number == other.as.number;
    case value_type::OBJECT:
        auto* str_a = static_cast<obj_string*>(this->as.object);
        auto* str_b = static_cast<obj_string*>(other.as.object);
        return str_a->length == str_b->length
            && std::strcmp(str_a->chars, str_b->chars) == 0;
    }
}

void lox::print_value(value value)
{
    switch (value.type)
    {
    case value_type::BOOL:
        std::cout << (value.as.boolean ? "true" : "false");
        break;

    case value_type::NIL:
        std::cout << "nil";
        break;

    case value_type::NUMBER:
        std::cout << value.as.number;
        break;

    case value_type::OBJECT:
        switch (value.as.object->type)
        {
        case obj_type::STRING:
            std::cout << '"' << static_cast<obj_string*>(value.as.object)->chars << '"';
            break;
        }
        break;
    }
}
