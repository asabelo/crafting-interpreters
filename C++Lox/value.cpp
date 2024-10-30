
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
    }
}
