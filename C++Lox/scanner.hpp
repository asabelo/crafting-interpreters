
#pragma once

#include "common.hpp"

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

        bool is_at_end();

        token make_token(token_type type);

        token error_token(const char* message);

        char advance();

        bool match(char expected);
        
        char peek();

        char peek_next();

        void skip_whitespace();

        token make_string();

        bool is_digit(char c);

        token number();

        bool is_alpha(char c);

        token_type check_keyword(int start, int length, const char* rest, token_type type);

        token_type identifier_type();

        token identifier();

    public:

        scanner(const char* source);

        token scan_token();
    };
}
