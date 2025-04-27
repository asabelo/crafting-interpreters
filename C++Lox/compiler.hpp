
#pragma once

#include <set>
#include <unordered_map>

#include "chunk.hpp"
#include "common.hpp"
#include "object.hpp"
#include "parser.hpp"
#include "scanner.hpp"

namespace lox
{
    enum class precedence : int
    {
        NONE,
        ASSIGNMENT,  // =
        OR,          // or
        AND,         // and
        EQUALITY,    // == !=
        COMPARISON,  // < > <= >=
        TERM,        // + -
        FACTOR,      // * /
        UNARY,       // ! -
        CALL,        // . ()
        PRIMARY
    };

    struct parse_rule
    {
        std::optional<std::function<void()>> prefix = std::nullopt;
        std::optional<std::function<void()>> infix  = std::nullopt;
        precedence precedence                       = precedence::NONE;
    };

    class compiler
    {
        chunk& m_chunk;
        parser m_parser;
        std::unordered_map<std::string_view, std::shared_ptr<obj_string>> m_strings;

        chunk& current_chunk();

        void emit(uint8_t byte);
        void emit(uint8_t first_byte, uint8_t second_byte);
        void emit(value constant);

        uint8_t make_constant(value value);

        void print_statement();

        void statement();

        void declaration();

        void expression();

        void var_declaration();

        void expression_statement();

        void number();

        void string();

        void named_variable(token name);

        void variable();

        void grouping();

        void unary();

        void binary();

        void literal();

        const parse_rule& get_rule(token_type type) const;

        void parse_precedence(precedence precedence);

        uint8_t identifier_constant(token name);

        uint8_t parse_variable(std::string_view error_message);

        void define_variable(uint8_t global);

        const std::unordered_map<token_type, parse_rule> rules // sorry
        {
            { token_type::LEFT_PAREN,    { std::bind(&compiler::grouping, this), std::nullopt, precedence::NONE } },
            { token_type::RIGHT_PAREN,   { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::LEFT_BRACE,    { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::RIGHT_BRACE,   { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::COMMA,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::DOT,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::MINUS,         { std::bind(&compiler::unary, this), std::bind(&compiler::binary, this), precedence::TERM} },
            { token_type::PLUS,          { std::nullopt, std::bind(&compiler::binary, this), precedence::TERM} },
            { token_type::SEMICOLON,     { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::SLASH,         { std::nullopt, std::bind(&compiler::binary, this), precedence::FACTOR} },
            { token_type::STAR,          { std::nullopt, std::bind(&compiler::binary, this), precedence::FACTOR} },
            { token_type::BANG,          { std::bind(&compiler::unary, this), std::nullopt, precedence::NONE } },
            { token_type::BANG_EQUAL,    { std::nullopt, std::bind(&compiler::binary, this), precedence::EQUALITY } },
            { token_type::EQUAL,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::EQUAL_EQUAL,   { std::nullopt, std::bind(&compiler::binary, this), precedence::EQUALITY } },
            { token_type::GREATER,       { std::nullopt, std::bind(&compiler::binary, this), precedence::COMPARISON } },
            { token_type::GREATER_EQUAL, { std::nullopt, std::bind(&compiler::binary, this), precedence::COMPARISON } },
            { token_type::LESS,          { std::nullopt, std::bind(&compiler::binary, this), precedence::COMPARISON } },
            { token_type::LESS_EQUAL,    { std::nullopt, std::bind(&compiler::binary, this), precedence::COMPARISON } },
            { token_type::IDENTIFIER,    { std::bind(&compiler::variable, this), std::nullopt, precedence::NONE}},
            { token_type::STRING,        { std::bind(&compiler::string, this), std::nullopt, precedence::NONE } },
            { token_type::NUMBER,        { std::bind(&compiler::number, this), std::nullopt, precedence::NONE } },
            { token_type::AND,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::CLASS,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::ELSE,          { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::FALSE,         { std::bind(&compiler::literal, this), std::nullopt, precedence::NONE}},
            { token_type::FOR,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::FUN,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::IF,            { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::NIL,           { std::bind(&compiler::literal, this), std::nullopt, precedence::NONE } },
            { token_type::OR,            { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::PRINT,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::RETURN,        { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::SUPER,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::THIS,          { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::TRUE,          { std::bind(&compiler::literal, this), std::nullopt, precedence::NONE } },
            { token_type::VAR,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::WHILE,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::ERROR,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::END_OF_FILE,   { std::nullopt, std::nullopt, precedence::NONE } },
        };

    public:

        compiler(const std::string_view source, chunk& chunk, std::unordered_map<std::string_view, std::shared_ptr<obj_string>>& strings);

        bool compile();
    };
}
