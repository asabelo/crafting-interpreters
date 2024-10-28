
#include "vm.hpp"

#include "compiler.hpp"
#include "debug.hpp"

lox::vm::vm(lox::chunk& chunk) 
    : m_chunk{ chunk }
    , m_ip{ 0 }
    , m_stack{}
{
}

lox::interpret_result lox::vm::run()
{
    const auto read_byte = [this]()
    {
        return m_chunk.get(m_ip++);
    };

    const auto read_constant = [this, &read_byte]()
    {
        return m_chunk.constants().get(read_byte());
    };

    const auto binary_op = [this](std::function<value(value, value)> op)
    {
        const auto b = m_stack.pop();
        const auto a = m_stack.pop();
        return m_stack.push(op(a, b));
    };

#ifdef _DEBUG
    std::cout << "\n== trace ==";
#endif // _DEBUG

    while (true)
    {
#ifdef _DEBUG
        std::cout << "          ";

        for (lox::stack<value>::idx_t i = 0; i < m_stack.count(); ++i)
        {
            std::cout << "[ ";

            print_value(m_stack.get(i));

            std::cout << " ]";
        }

        std::cout << '\n';

        disassemble_instruction(m_chunk, m_ip);
#endif // DEBUG

        switch (read_byte())
        {
        case op_code::OP_CONSTANT:
            m_stack.push(read_constant());
            break;

        case op_code::OP_ADD:
            binary_op(std::plus{});
            break;

        case op_code::OP_SUBTRACT:
            binary_op(std::minus{});
            break;

        case op_code::OP_MULTIPLY:
            binary_op(std::multiplies{});
            break;

        case op_code::OP_DIVIDE:
            binary_op(std::divides{});
            break;

        case op_code::OP_NEGATE:
            m_stack.push(-m_stack.pop());
            break;

        case op_code::OP_RETURN:
            print_value(m_stack.pop());
            std::cout << '\n';
            return interpret_result::OK;
        }
    }
}

lox::interpret_result lox::vm::interpret(const std::string_view source)
{
    lox::compiler compiler{ source, m_chunk };

    if (!compiler.compile())
    {
        return interpret_result::COMPILE_ERROR;
    }

    auto result = run();

    return result;
}
