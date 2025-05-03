
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
    current_chunk().emplace_back(byte, m_parser.previous().line);
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

void lox::compiler::emit_loop(std::vector<op_info>::size_type loopStart)
{
    emit(op_code::OP_LOOP);

    auto offset = m_chunk.size() - loopStart + 2;

    if (offset > std::numeric_limits<uint16_t>::max())
    {
        m_parser.error("Loop body too large.");
    }

    emit((offset >> 8) & 0xff);
    emit(offset & 0xff);
}

std::vector<lox::op_info>::size_type lox::compiler::emit_jump(op_code jump)
{
    emit(jump);
    emit(0xff);
    emit(0xff);
    return m_chunk.size() - 2;
}

uint8_t lox::compiler::make_constant(value value)
{
    // TODO have array throw when adding at max index

    auto& constants = current_chunk().constants();

    constants.push_back(value);
    return static_cast<uint8_t>(constants.size() - 1);
}

void lox::compiler::patch_jump(std::vector<lox::op_info>::size_type offset)
{
    // -2 to adjust for the bytecode for the jump offset itself.
    auto jump = m_chunk.size() - offset - 2;

    if (jump > std::numeric_limits<uint16_t>::max())
    {
        m_parser.error("Too much code to jump over.");
    }

    m_chunk.at(offset    ).op = static_cast<uint8_t>(0xff & (jump >> 8));
    m_chunk.at(offset + 1).op = static_cast<uint8_t>(0xff &  jump      );
}

void lox::compiler::print_statement()
{
    expression();
    
    m_parser.consume(token_type::SEMICOLON, "Expect ';' after value.");

    emit(op_code::OP_PRINT);
}

void lox::compiler::while_statement()
{
    auto loop_start = m_chunk.size();

    m_parser.consume(token_type::LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    m_parser.consume(token_type::RIGHT_PAREN, "Expect ')' after condition.");

    auto exit_jump = emit_jump(op_code::OP_JUMP_IF_FALSE);
    emit(op_code::OP_POP);
    statement();
    emit_loop(loop_start);

    patch_jump(exit_jump);
    emit(op_code::OP_POP);
}

void lox::compiler::statement()
{
    if (m_parser.match(token_type::PRINT))
    {
        print_statement();
    }
    else if (m_parser.match(token_type::IF))
    {
        if_statement();
    }
    else if (m_parser.match(token_type::WHILE))
    {
        while_statement();
    }
    else if (m_parser.match(token_type::LEFT_BRACE))
    {
        begin_scope();
        block();
        end_scope();
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

void lox::compiler::block()
{
    while (not m_parser.check(token_type::RIGHT_BRACE) and not m_parser.check(token_type::END_OF_FILE))
    {
        declaration();
    }

    m_parser.consume(token_type::RIGHT_BRACE, "Expect '}' after block.");
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

void lox::compiler::if_statement()
{
    m_parser.consume(token_type::LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    m_parser.consume(token_type::RIGHT_PAREN, "Expect ')' after condition.");

    auto then_jump = emit_jump(op_code::OP_JUMP_IF_FALSE);
    emit(op_code::OP_POP);
    statement();

    auto else_jump = emit_jump(op_code::OP_JUMP);
    patch_jump(then_jump);
    emit(op_code::OP_POP);

    if (m_parser.match(token_type::ELSE))
    {
        statement();
        patch_jump(else_jump);
    }
}

void lox::compiler::number(bool)
{
    auto number = std::stod(std::string{ m_parser.previous().text });

    emit(value::from(number));
}

void lox::compiler::string(bool)
{
    const auto text = m_parser.previous().text;

    auto string_contents = text.substr(1, text.length() - 2);

    if (m_strings.contains(string_contents))
    {
        emit(value::from(m_strings.at(string_contents)));
    }
    else
    {
        const auto ptr = std::make_shared<obj_string>(string_contents);

        m_strings[string_contents] = ptr;

        emit(value::from(ptr));
    }
}

void lox::compiler::named_variable(token name, bool can_assign)
{
    uint8_t get, set, arg;

    auto local = resolve_local(name);

    if (local != -1)
    {
        arg = static_cast<uint8_t>(local);
        get = op_code::OP_GET_LOCAL;
        set = op_code::OP_SET_LOCAL;
    }
    else
    {
        arg = identifier_constant(name);
        get = op_code::OP_GET_GLOBAL;
        set = op_code::OP_SET_GLOBAL;
    }

    if (can_assign and m_parser.match(token_type::EQUAL))
    {
        expression();

        emit(set, arg);
    }
    else
    {
        emit(get, arg);
    }
}

void lox::compiler::variable(bool can_assign)
{
    named_variable(m_parser.previous(), can_assign);
}

void lox::compiler::grouping(bool)
{
    expression();

    m_parser.consume(token_type::RIGHT_PAREN, "Expect ')' after expression.");
}

void lox::compiler::unary(bool)
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

void lox::compiler::binary(bool)
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

void lox::compiler::literal(bool)
{
    switch (m_parser.previous().type)
    {
    case token_type::FALSE: emit(op_code::OP_FALSE); break;
    case token_type::NIL:   emit(op_code::OP_NIL);   break;
    case token_type::TRUE:  emit(op_code::OP_TRUE);  break;
    default: return; // Unreachable
    }
}

void lox::compiler::begin_scope()
{
    ++scope_depth;
}

void lox::compiler::end_scope()
{
    --scope_depth;

    while (m_locals.size() > 0 and m_locals.at(m_locals.size() - 1).depth > scope_depth)
    {
        emit(op_code::OP_POP);

        m_locals.pop_back(); // ?
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

    bool can_assign = precedence <= precedence::ASSIGNMENT;
    prefix_rule.value()(can_assign);

    while (precedence <= get_rule(m_parser.current().type).precedence)
    {
        m_parser.advance();

        const auto& infix_rule = get_rule(m_parser.previous().type).infix;
        
        if (infix_rule.has_value())
        {
            infix_rule.value()(can_assign);
        }

        if (can_assign and m_parser.match(token_type::EQUAL))
        {
            m_parser.error("Invalid assignment target.");
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
        auto new_identifier = std::make_shared<obj_string>(name.text);

        m_strings[name.text] = new_identifier;

        return make_constant(value::from(new_identifier));
    }

    return make_constant(value::from(std::make_shared<obj_string>(name.text)));
}

bool lox::compiler::identifiers_equal(const token& a, const token& b)
{
    return a.text == b.text;
}

int lox::compiler::resolve_local(const token& name)
{
    if (auto size = m_locals.size(); size > 0)
    {
        const auto crbegin = m_locals.crbegin();

        for (auto crit = crbegin; crit != m_locals.crend(); ++crit)
        {
            const auto& local = *crit;

            if (identifiers_equal(name, local.name))
            {
                if (local.depth == -1)
                {
                    m_parser.error("Can't read local variable in its own initializer.");
                }

                const auto local_index = std::distance(crbegin, crit);

                return static_cast<int>(local_index);
            }
        }
    }

    return -1;
}

void lox::compiler::add_local(token name)
{
    if (m_locals.size() > std::numeric_limits<uint8_t>::max())
    {
        m_parser.error("Too many local variables in function.");

        return;
    }

    m_locals.emplace_back(name, -1);
}

void lox::compiler::declare_variable()
{
    if (scope_depth == 0) return;

    auto name = m_parser.previous();

    if (auto size = m_locals.size(); size > 0)
    {
        for (auto crit = m_locals.crbegin(); crit != m_locals.crend(); ++crit)
        {
            const auto& local = *crit;

            if (local.depth != -1 and local.depth < scope_depth)
            {
                break;
            }

            if (identifiers_equal(name, local.name))
            {
                m_parser.error("Already a variable with this name in this scope.");
            }
        }
    }

    add_local(name);
}

uint8_t lox::compiler::parse_variable(std::string_view message)
{
    m_parser.consume(token_type::IDENTIFIER, message);

    declare_variable();

    if (scope_depth > 0) return 0;

    return identifier_constant(m_parser.previous());
}

void lox::compiler::mark_initialized()
{
    m_locals.back().depth = scope_depth;
}

void lox::compiler::define_variable(uint8_t global)
{
    if (scope_depth > 0)
    {
        mark_initialized();

        return;
    }

    emit(op_code::OP_DEFINE_GLOBAL, global);
}

void lox::compiler::and_(bool)
{
    auto end_jump = emit_jump(op_code::OP_JUMP_IF_FALSE);

    emit(OP_POP);
    parse_precedence(precedence::AND);

    patch_jump(end_jump);
}

void lox::compiler::or_(bool)
{
    auto else_jump = emit_jump(op_code::OP_JUMP_IF_FALSE);
    auto end_jump = emit_jump(op_code::OP_JUMP);

    patch_jump(else_jump);
    emit(op_code::OP_POP);

    parse_precedence(precedence::OR);
    patch_jump(end_jump);
}

lox::compiler::compiler(const std::string_view source, chunk& chunk, std::unordered_map<std::string_view, std::shared_ptr<obj_string>>& strings)
    : m_chunk{ chunk }
    , m_parser{ source }
    , m_strings{ strings }
    , m_locals{}
    , scope_depth{ 0 }
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
