
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

lox::obj_string::obj_string(const obj_string& other)
    : obj{ obj_type::STRING }
{
    m_length = other.m_length;
    m_chars = allocate_array<char>(m_length);

    std::copy(other.m_chars.get(), other.m_chars.get() + other.m_length, m_chars.get());
}

lox::obj_string& lox::obj_string::operator=(obj_string other)
{
    std::swap(m_length, other.m_length);
    std::swap(m_chars, other.m_chars);

    return *this;
}

lox::obj_string::obj_string(obj_string&& other) noexcept
    : obj{ obj_type::STRING }
{
    m_length = other.m_length;
    m_chars.swap(other.m_chars);

    other.m_length = 0;
    other.m_chars.reset();
}

lox::obj_string& lox::obj_string::operator=(obj_string&& other) noexcept
{
    if (&other != this)
    {
        m_length = other.m_length;
        m_chars.swap(other.m_chars);

        other.m_length = 0;
        other.m_chars.reset();
    }

    return *this;
}

std::size_t lox::obj_string::length() const
{
    return m_length;
}

void lox::obj_string::concat(const obj_string& other)
{
    auto old_length = m_length;
    auto new_length = m_length = old_length + other.m_length;

    lox::grow_array(m_chars, old_length, new_length + 1);
    m_chars[new_length] = '\0';

    std::copy(other.m_chars.get(), other.m_chars.get() + other.m_length, m_chars.get() + old_length);
}

void lox::obj_string::print() const
{
    std::cout << '"' << m_chars << '"';
}

bool lox::obj_string::equals(const obj_string& other) const
{
    return false;
    return m_length == other.m_length
        && std::strcmp(m_chars.get(), other.m_chars.get()) == 0;
}

lox::obj::obj(obj_type type)
    : m_type{ type }
    , m_next{ nullptr }
{
}

lox::obj_type lox::obj::type() const
{
    return m_type;
}
