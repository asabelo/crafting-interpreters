
#include "object.hpp"

#include "memory.hpp"
#include "vm.hpp"

lox::obj_string::obj_string(std::string_view text)
    : obj{ obj_type::STRING }
{
    m_length = text.length();
    m_chars = allocate_array<char>(text.length() + 1);
    m_chars[m_length] = '\0';

    std::copy(text.cbegin(), text.cend(), m_chars.get());
}

std::size_t lox::obj_string::length() const
{
    return m_length;
}

char* lox::obj_string::chars() const
{
    return m_chars.get();
}

void lox::obj_string::concat(const obj_string& other)
{
    auto old_length = m_length;
    auto new_length = m_length = old_length + other.m_length;

    lox::resize_array(m_chars, old_length, new_length + 1);
    m_chars[new_length] = '\0';

    std::copy(other.m_chars.get(), other.m_chars.get() + other.m_length, m_chars.get() + old_length);
}

void lox::obj_string::print() const
{
    std::cout << '"' << m_chars << '"';
}

lox::obj::obj(obj_type type)
    : m_type{ type }
{
}

lox::obj_type lox::obj::type() const
{
    return m_type;
}
