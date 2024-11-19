
#include "value.hpp"

lox::value::value()
    : m_type{ value_type::NIL }
    , m_inner{}
{
}

lox::value::value(bool value)
    : m_type{ value_type::BOOL }
    , m_inner{ value }
{
}

lox::value::value(double value)
    : m_type{ value_type::NUMBER }
    , m_inner{ value }
{
}

lox::value::value(std::shared_ptr<obj> value)
    : m_type{ value_type::OBJECT }
    , m_inner{ value }
{
}

lox::value lox::value::nil()
{
    return {};
}

lox::value lox::value::from(bool value)
{
    return { value };
}

lox::value lox::value::from(double value)
{
    return { value };
}

lox::value lox::value::from(std::shared_ptr<obj> value)
{
    return { value };
}

bool lox::value::is_nil() const
{
    return m_type == value_type::NIL;
}

bool lox::value::is_boolean() const
{
    return m_type == value_type::BOOL;
}

bool lox::value::is_number() const
{
    return m_type == value_type::NUMBER;
}

bool lox::value::is_object() const
{
    return m_type == value_type::OBJECT;
}

bool lox::value::is_string() const
{
    return is_object() && this->as_object()->type() == obj_type::STRING;
}

bool lox::value::is_falsey() const
{
    return is_nil() || (is_boolean() && !std::get<bool>(m_inner));
}

bool lox::value::as_boolean() const
{
    return std::get<bool>(m_inner);
}

double lox::value::as_number() const
{
    return std::get<double>(m_inner);
}

std::shared_ptr<lox::obj> lox::value::as_object() const
{
    return std::get<std::shared_ptr<obj>>(m_inner);
}

bool lox::value::equals(const value& other) const
{
    if (m_type != other.m_type) return false;

    switch (m_type)
    {
    case value_type::BOOL:   return this->as_boolean() == other.as_boolean();
    case value_type::NIL:    return true;
    case value_type::NUMBER: return this->as_number() == other.as_number();
    case value_type::OBJECT: return this->as_object()->equals(*other.as_object());
    }

    return false;
}

void lox::value::print() const
{
    switch (m_type)
    {
    case value_type::BOOL:
        std::cout << (this->as_boolean() ? "true" : "false");
        break;

    case value_type::NIL:
        std::cout << "nil";
        break;

    case value_type::NUMBER:
        std::cout << this->as_number();
        break;

    case value_type::OBJECT:
        this->as_object()->print();
        break;
    }
}
