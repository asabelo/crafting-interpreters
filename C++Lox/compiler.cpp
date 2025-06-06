
#include "compiler.hpp"

#ifdef _DEBUG
#include "debug.hpp"
#endif // _DEBUG

lox::chunk& lox::compiler::current_chunk()
{
    return m_chunk;
}

void lox::compiler::emit(uint8_t byte)
{
    current_chunk().add({ byte, m_parser.previous().line });
}

void lox::compiler::emit(uint8_t first_byte, uint8_t second_byte)
{
    emit(first_byte);
    emit(second_byte);
}

void lox::compiler::emit(value constant)
{
    emit(op_code::OP_CONSTANT, make_constant(constant));
}

uint8_t lox::compiler::make_constant(value value)
{
    // TODO have array throw when adding at max index

    return current_chunk().constants().add(value);
}

void lox::compiler::print_statement()
{
    expression();
    
    m_parser.consume(token_type::SEMICOLON, "Expect ';' after value.");

    emit(op_code::OP_PRINT);
}

void lox::compiler::statement()
{
    if (m_parser.match(token_type::PRINT))
    {
        print_statement();
    }
    else
    {
        expression_statement();
    }
}

void lox::compiler::declaration()
{
    if (m_parser.match(token_type::VAR))
    {
        var_declaration();
    }
    else
    {
        statement();
    }

    m_parser.synchronize_if_panicking();
}

void lox::compiler::expression()
{
    parse_precedence(precedence::ASSIGNMENT);
}

void lox::compiler::var_declaration()
{
    auto global = parse_variable("Expect variable name.");

    if (m_parser.match(token_type::EQUAL))
    {
        expression();
    }
    else
    {
        emit(op_code::OP_NIL);
    }

    m_parser.consume(token_type::SEMICOLON, "Expect ';' after variable declaration.");

    define_variable(global);
}

void lox::compiler::expression_statement()
{
    expression();

    m_parser.consume(token_type::SEMICOLON, "Expect ';' after expression.");

    emit(op_code::OP_POP);
}

void lox::compiler::number()
{
    auto number = std::stod(std::string{ m_parser.previous().text });

    emit(value::from(number));
}

void lox::compiler::string()
{
    const auto text = m_parser.previous().text;

    auto string_contents = text.substr(1, text.length() - 2);

    if (m_strings.contains(string_contents))
    {
        emit(value::from(m_strings.at(string_contents)));
    }
    else
    {
        const auto ptr = allocate_shared<obj_string>(string_contents);

        m_strings[string_contents] = ptr;

        emit(value::from(ptr));
    }
}

void lox::compiler::named_variable(token name)
{
    uint8_t arg = identifier_constant(name);

    emit(op_code::OP_GET_GLOBAL, arg);
}

void lox::compiler::variable()
{
    named_variable(m_parser.previous());
}

void lox::compiler::grouping()
{
    expression();

    m_parser.consume(token_type::RIGHT_PAREN, "Expect ')' after expression.");
}

void lox::compiler::unary()
{
    auto operator_type = m_parser.previous().type;

    parse_precedence(precedence::UNARY);

    switch (operator_type)
    {
    case token_type::BANG:  emit(op_code::OP_NOT);    break;
    case token_type::MINUS: emit(op_code::OP_NEGATE); break;
    default: return; // Unreachable.
    }
}

void lox::compiler::binary()
{
    auto operator_type = m_parser.previous().type;

    const auto& rule = get_rule(operator_type);

    parse_precedence(static_cast<precedence>(static_cast<int>(rule.precedence) + 1));

    switch (operator_type)
    {
    case token_type::BANG_EQUAL:    emit(op_code::OP_EQUAL, op_code::OP_NOT);   break;
    case token_type::EQUAL_EQUAL:   emit(op_code::OP_EQUAL);                    break;
    case token_type::GREATER:       emit(op_code::OP_GREATER);                  break;
    case token_type::GREATER_EQUAL: emit(op_code::OP_LESS, op_code::OP_NOT);    break;
    case token_type::LESS:          emit(op_code::OP_LESS);                     break;
    case token_type::LESS_EQUAL:    emit(op_code::OP_GREATER, op_code::OP_NOT); break;
    case token_type::PLUS:          emit(op_code::OP_ADD);                      break;
    case token_type::MINUS:         emit(op_code::OP_SUBTRACT);                 break;
    case token_type::STAR:          emit(op_code::OP_MULTIPLY);                 break;
    case token_type::SLASH:         emit(op_code::OP_DIVIDE);                   break;
    default: return; // Unreachable.
    }
}

void lox::compiler::literal()
{
    switch (m_parser.previous().type)
    {
    case token_type::FALSE: emit(op_code::OP_FALSE); break;
    case token_type::NIL:   emit(op_code::OP_NIL);   break;
    case token_type::TRUE:  emit(op_code::OP_TRUE);  break;
    default: return; // Unreachable
    }
}

const lox::parse_rule& lox::compiler::get_rule(token_type type) const
{
    return rules.at(type);
}

void lox::compiler::parse_precedence(precedence precedence)
{
    m_parser.advance();

    const auto& prefix_rule = get_rule(m_parser.previous().type).prefix;

    if (!prefix_rule.has_value())
    {
        m_parser.error("Expect expression.");

        return;
    }

    prefix_rule.value()();

    while (precedence <= get_rule(m_parser.current().type).precedence)
    {
        m_parser.advance();

        const auto& infix_rule = get_rule(m_parser.previous().type).infix;
        
        if (infix_rule.has_value())
        {
            infix_rule.value()();
        }
    }
}

uint8_t lox::compiler::identifier_constant(token name)
{
    if (m_strings.contains(name.text))
    {
        return make_constant(value::from(m_strings.at(name.text)));
    }
    else
    {
        auto blep = allocate_shared<obj_string>(name.text);
        m_strings[name.text] = blep;
        return make_constant(value::from(blep));
    }

    return make_constant(value::from(allocate_shared<obj_string>(name.text)));
}

uint8_t lox::compiler::parse_variable(std::string_view message)
{
    m_parser.consume(token_type::IDENTIFIER, message);

    return identifier_constant(m_parser.previous());
}

void lox::compiler::define_variable(uint8_t global)
{
    emit(op_code::OP_DEFINE_GLOBAL, global);
}

lox::compiler::compiler(const std::string_view source, chunk& chunk, std::unordered_map<std::string_view, std::shared_ptr<obj_string>>& strings)
    : m_chunk{ chunk }
    , m_parser{ source }
    , m_strings{ strings }
{
}

bool lox::compiler::compile()
{
    m_parser.advance();

    while (!m_parser.match(token_type::END_OF_FILE))
    {
        declaration();
    }

    m_parser.consume(token_type::END_OF_FILE, "Expect end of expression.");

    emit(op_code::OP_RETURN);

#ifdef _DEBUG
    if (!m_parser.had_error())
    {
        disassemble_chunk(m_chunk, "code");
    }
#endif // _DEBUG

    return !m_parser.had_error();
}
