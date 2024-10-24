
#pragma once

namespace lox
{
    enum token_type
    {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS,
        SEMICOLON, SLASH, STAR,
        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,
        // Literals.
        IDENTIFIER, STRING, NUMBER,
        // Keywords.
        AND, CLASS, ELSE, FALSE,
        FOR, FUN, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS,
        TRUE, VAR, WHILE,

        ERROR, END_OF_FILE
    };

    struct token
    {
        token_type type;
        const char* start;
        int length;
        int line;
    };

    class scanner
    {
        const char* m_start;

        const char* m_current;

        int m_line;

        bool is_at_end()
        {
            return *m_current == '\0';
        }

        token make_token(token_type type)
        {
            return
            {
                type,
                m_start,
                static_cast<int>(m_current - m_start),
                m_line
            };
        }

        token error_token(const char* message)
        {
            return
            {
                token_type::ERROR,
                message,
                static_cast<int>(std::strlen(message)),
                m_line
            };
        }

        char advance()
        {
            m_current++;

            return m_current[-1];
        }

        bool match(char expected)
        {
            if (is_at_end()) return false;

            if (*m_current != expected) return false;

            m_current++;

            return true;
        }
        
        char peek()
        {
            return *m_current;
        }

        char peek_next()
        {
            if (is_at_end()) return '\0';

            return m_current[1];
        }

        void skip_whitespace()
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

        token make_string()
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

        bool is_digit(char c)
        {
            return c >= '0' && c <= '9';
        }

        token number()
        {
            while (is_digit(peek())) advance();

            // Look for a fractional part.
            if (peek() == '.' && is_digit(peek_next())) {
                // Consume the ".".
                advance();

                while (is_digit(peek())) advance();
            }

            return make_token(token_type::NUMBER);
        }

        bool is_alpha(char c)
        {
            return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '_';
        }

        token_type check_keyword(int start, int length, const char* rest, token_type type)
        {
            if (m_current - m_start == start + length 
                && std::memcmp(m_start + start, rest, length) == 0)
            {
                return type;
            }

            return token_type::IDENTIFIER;
        }

        token_type identifier_type()
        {
            switch (m_start[0])
            {
            case 'a': return check_keyword(1, 2, "nd", token_type::AND);
            case 'c': return check_keyword(1, 4, "lass", token_type::CLASS);
            case 'e': return check_keyword(1, 3, "lse", token_type::ELSE);
            case 'f':
                if (m_current - m_start > 1)
                {
                    switch (m_start[1]) 
                    {
                    case 'a': return check_keyword(2, 3, "lse", token_type::FALSE);
                    case 'o': return check_keyword(2, 1, "r", token_type::FOR);
                    case 'u': return check_keyword(2, 1, "n", token_type::FUN);
                    }
                }
                break;
            case 'i': return check_keyword(1, 1, "f", token_type::IF);
            case 'n': return check_keyword(1, 2, "il", token_type::NIL);
            case 'o': return check_keyword(1, 1, "r", token_type::OR);
            case 'p': return check_keyword(1, 4, "rint", token_type::PRINT);
            case 'r': return check_keyword(1, 5, "eturn", token_type::RETURN);
            case 's': return check_keyword(1, 4, "uper", token_type::SUPER);
            case 't':
                if (m_current - m_start > 1) 
                {
                    switch (m_start[1]) 
                    {
                    case 'h': return check_keyword(2, 2, "is", token_type::THIS);
                    case 'r': return check_keyword(2, 2, "ue", token_type::TRUE);
                    }
                }
                break;
            case 'v': return check_keyword(1, 2, "ar", token_type::VAR);
            case 'w': return check_keyword(1, 4, "hile", token_type::WHILE);
            }
            return token_type::IDENTIFIER;
        }

        token identifier() 
        {
            while (is_alpha(peek()) || is_digit(peek())) advance();

            return make_token(identifier_type());
        }

    public:

        scanner(const char* source)
        {
            m_start = m_current = source;
            m_line = 1;
        }

        token scan_token()
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
    };
}
