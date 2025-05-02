
#pragma once

#include <unordered_map>
#include <vector>

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
        std::optional<std::function<void(bool)>> prefix = std::nullopt;
        std::optional<std::function<void(bool)>> infix  = std::nullopt;
        precedence precedence                           = precedence::NONE;
    };

    struct local
    {
        token name;
        int depth;
    };

    class compiler
    {
        chunk& m_chunk;
        parser m_parser;
        std::unordered_map<std::string_view, std::shared_ptr<obj_string>> m_strings;
        std::vector<local> m_locals;
        int scope_depth;

        chunk& current_chunk();

        void emit(uint8_t byte);
        void emit(uint8_t first_byte, uint8_t second_byte);
        void emit(value constant);

        uint8_t make_constant(value value);

        void print_statement();

        void statement();

        void declaration();

        void expression();

        void block();

        void var_declaration();

        void expression_statement();

        void number(bool);

        void string(bool);

        void named_variable(token name, bool can_assign);

        void variable(bool can_assign);

        void grouping(bool);

        void unary(bool);

        void binary(bool);

        void literal(bool);

        void begin_scope();

        void end_scope();

        const parse_rule& get_rule(token_type type) const;

        void parse_precedence(precedence precedence);

        uint8_t identifier_constant(token name);

        bool identifiers_equal(const token& a, const token& b);

        int resolve_local(const token& name);

        void add_local(token name);

        void declare_variable();

        uint8_t parse_variable(std::string_view error_message);

        void mark_initialized();

        void define_variable(uint8_t global);

        const std::unordered_map<token_type, parse_rule> rules // sorry
        {
            { token_type::LEFT_PAREN,    { std::bind(&compiler::grouping, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
            { token_type::RIGHT_PAREN,   { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::LEFT_BRACE,    { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::RIGHT_BRACE,   { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::COMMA,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::DOT,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::MINUS,         { std::bind(&compiler::unary, this, std::placeholders::_1), std::bind(&compiler::binary, this, std::placeholders::_1), precedence::TERM} },
            { token_type::PLUS,          { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::TERM} },
            { token_type::SEMICOLON,     { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::SLASH,         { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::FACTOR} },
            { token_type::STAR,          { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::FACTOR} },
            { token_type::BANG,          { std::bind(&compiler::unary, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
            { token_type::BANG_EQUAL,    { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::EQUALITY } },
            { token_type::EQUAL,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::EQUAL_EQUAL,   { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::EQUALITY } },
            { token_type::GREATER,       { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::COMPARISON } },
            { token_type::GREATER_EQUAL, { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::COMPARISON } },
            { token_type::LESS,          { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::COMPARISON } },
            { token_type::LESS_EQUAL,    { std::nullopt, std::bind(&compiler::binary, this, std::placeholders::_1), precedence::COMPARISON } },
            { token_type::IDENTIFIER,    { std::bind(&compiler::variable, this, std::placeholders::_1), std::nullopt, precedence::NONE}},
            { token_type::STRING,        { std::bind(&compiler::string, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
            { token_type::NUMBER,        { std::bind(&compiler::number, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
            { token_type::AND,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::CLASS,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::ELSE,          { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::FALSE,         { std::bind(&compiler::literal, this, std::placeholders::_1), std::nullopt, precedence::NONE}},
            { token_type::FOR,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::FUN,           { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::IF,            { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::NIL,           { std::bind(&compiler::literal, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
            { token_type::OR,            { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::PRINT,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::RETURN,        { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::SUPER,         { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::THIS,          { std::nullopt, std::nullopt, precedence::NONE } },
            { token_type::TRUE,          { std::bind(&compiler::literal, this, std::placeholders::_1), std::nullopt, precedence::NONE } },
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
