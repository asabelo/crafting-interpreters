
#pragma once

#include "common.hpp"

namespace lox
{
    enum class token_type : int
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
        token_type       type;
        std::string_view text;
        int              line;
    };

    class scanner
    {
        const std::string_view m_source;

        std::string::size_type m_start;

        std::string::size_type m_current;

        int m_line;

        bool is_at_end();

        token make_token(const token_type type);

        token error_token(const std::string_view message);

        char advance();

        bool match(char expected);
        
        char peek();

        char peek_next();

        void skip_whitespace();

        token make_string();

        bool is_digit(char c);

        token number();

        bool is_alpha(char c);

        token_type check_keyword(const std::size_t start, const std::string_view rest, const token_type type);

        token_type identifier_type();

        token identifier();

    public:

        scanner(const std::string_view source);

        token scan_token();
    };
}
