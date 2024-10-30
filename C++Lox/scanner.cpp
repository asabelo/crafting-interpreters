
#include "scanner.hpp"

bool lox::scanner::is_at_end()
{
    return m_current == m_source.length();
}

lox::token lox::scanner::make_token(const lox::token_type type)
{
    return
    {
        type,
        m_source.substr(m_start, m_current - m_start),
        m_line
    };
}

lox::token lox::scanner::error_token(const std::string_view message)
{
    return
    {
        token_type::ERROR,
        message,
        m_line
    };
}

char lox::scanner::advance()
{
    m_current++;

    return m_source[m_current - 1];
}

bool lox::scanner::match(char expected)
{
    if (is_at_end()) return false;

    if (m_source[m_current] != expected) return false;

    m_current++;

    return true;
}

char lox::scanner::peek()
{
    if (is_at_end()) return '\0';

    return m_source[m_current];
}

char lox::scanner::peek_next()
{
    if (is_at_end() || m_current + 1 == m_source.length()) return '\0';

    return m_source[m_current + 1];
}

void lox::scanner::skip_whitespace()
{
    for (;;)
    {
        char c = peek();

        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            m_line++;
            advance();
            break;
        case '/':
            if (peek_next() == '/')
            {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !is_at_end()) advance();
            }
            else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}

lox::token lox::scanner::make_string()
{
    while (peek() != '"' && !is_at_end())
    {
        if (peek() == '\n') m_line++;

        advance();
    }

    if (is_at_end()) return error_token("Unterminated string.");

    // The closing quote.
    advance();

    return make_token(token_type::STRING);
}

bool lox::scanner::is_digit(char c)
{
    return c >= '0' && c <= '9';
}

lox::token lox::scanner::number()
{
    while (is_digit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && is_digit(peek_next()))
    {
        // Consume the ".".
        advance();

        while (is_digit(peek())) advance();
    }

    return make_token(token_type::NUMBER);
}

bool lox::scanner::is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

lox::token_type lox::scanner::check_keyword(const std::size_t start, const std::string_view rest, const token_type type)
{
    if (m_current - m_start == start + rest.length()
        && rest.compare(m_source.substr(m_start + start, rest.length())) == 0)
    {
        return type;
    }

    return token_type::IDENTIFIER;
}

lox::token_type lox::scanner::identifier_type()
{
    switch (m_source[m_start])
    {
    case 'a': return check_keyword(1, "nd", token_type::AND);
    case 'c': return check_keyword(1, "lass", token_type::CLASS);
    case 'e': return check_keyword(1, "lse", token_type::ELSE);
    case 'f':
        if (m_current - m_start > 1)
        {
            switch (m_source[m_start + 1])
            {
            case 'a': return check_keyword(2, "lse", token_type::FALSE);
            case 'o': return check_keyword(2, "r", token_type::FOR);
            case 'u': return check_keyword(2, "n", token_type::FUN);
            }
        }
        break;
    case 'i': return check_keyword(1, "f", token_type::IF);
    case 'n': return check_keyword(1, "il", token_type::NIL);
    case 'o': return check_keyword(1, "r", token_type::OR);
    case 'p': return check_keyword(1, "rint", token_type::PRINT);
    case 'r': return check_keyword(1, "eturn", token_type::RETURN);
    case 's': return check_keyword(1, "uper", token_type::SUPER);
    case 't':
        if (m_current - m_start > 1)
        {
            switch (m_source[m_start + 1])
            {
            case 'h': return check_keyword(2, "is", token_type::THIS);
            case 'r': return check_keyword(2, "ue", token_type::TRUE);
            }
        }
        break;
    case 'v': return check_keyword(1, "ar", token_type::VAR);
    case 'w': return check_keyword(1, "hile", token_type::WHILE);
    }

    return token_type::IDENTIFIER;
}

lox::token lox::scanner::identifier()
{
    while (is_alpha(peek()) || is_digit(peek())) advance();

    return make_token(identifier_type());
}

lox::scanner::scanner(const std::string_view source) : m_source{ source }
{
    m_start = m_current = 0;
    m_line = 1;
}

lox::token lox::scanner::scan_token()
{
    skip_whitespace();

    m_start = m_current;

    if (is_at_end()) return make_token(token_type::END_OF_FILE);

    char c = advance();

    if (is_alpha(c)) return identifier();

    if (is_digit(c)) return number();

    switch (c)
    {
    case '(': return make_token(token_type::LEFT_PAREN);
    case ')': return make_token(token_type::RIGHT_PAREN);
    case '{': return make_token(token_type::LEFT_BRACE);
    case '}': return make_token(token_type::RIGHT_BRACE);
    case ';': return make_token(token_type::SEMICOLON);
    case ',': return make_token(token_type::COMMA);
    case '.': return make_token(token_type::DOT);
    case '-': return make_token(token_type::MINUS);
    case '+': return make_token(token_type::PLUS);
    case '/': return make_token(token_type::SLASH);
    case '*': return make_token(token_type::STAR);
    case '!': return make_token(match('=') ? token_type::BANG_EQUAL : token_type::BANG);
    case '=': return make_token(match('=') ? token_type::EQUAL_EQUAL : token_type::EQUAL);
    case '<': return make_token(match('=') ? token_type::LESS_EQUAL : token_type::LESS);
    case '>': return make_token(match('=') ? token_type::GREATER_EQUAL : token_type::GREATER);
    case '"': return make_string();
    }

    return error_token("Unexpected character.");
}
