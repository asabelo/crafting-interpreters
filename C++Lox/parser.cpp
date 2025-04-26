
#include "parser.hpp"

lox::parser::parser(const std::string_view source)
    : m_scanner{ source }
    , m_had_error{ false }
    , m_panic_mode{ false }
{
}

lox::token lox::parser::current() const
{
    return m_current;
}

lox::token lox::parser::previous() const
{
    return m_previous;
}

void lox::parser::advance()
{
    m_previous = m_current;

    while (true)
    {
        m_current = m_scanner.scan_token();

        if (m_current.type != token_type::ERROR) break;

        error_at_current(m_current.text);
    }
}

void lox::parser::consume(const token_type type, const std::string_view message)
{
    if (m_current.type == type)
    {
        advance();

        return;
    }

    error_at_current(message);
}

bool lox::parser::check(const token_type type) const
{
    return m_current.type == type;
}

bool lox::parser::match(const token_type type)
{
    if (!check(type)) return false;

    advance();

    return true;
}

void lox::parser::error_at_current(const std::string_view message)
{
    error_at(m_current, message);
}

void lox::parser::error(const std::string_view message)
{
    error_at(m_current, message);
}

void lox::parser::error_at(const token& token, const std::string_view message)
{
    if (m_panic_mode) return;

    m_panic_mode = true;

    std::cerr << std::format("[line {}] Error", token.line);

    if (token.type == token_type::END_OF_FILE)
    {
        std::cerr << " at end";
    }
    else if (token.type == token_type::ERROR)
    {
        // nothign
    }
    else
    {
        std::cerr << std::format(" at '{}'", token.text);
    }

    std::cerr << std::format(": {}\n", message);

    m_had_error = true;
}

bool lox::parser::had_error() const
{
    return m_had_error;
}
