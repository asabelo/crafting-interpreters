
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

    const auto binary_op = [this](std::function<double(double, double)> op)
    {
        if (!m_stack.peek(0).is_number() || !m_stack.peek(1).is_number())
        {
            runtime_error("Operands must be numbers.");
            throw interpret_result::RUNTIME_ERROR;
        }
        const auto b = m_stack.pop().as.number;
        const auto a = m_stack.pop().as.number;
        return m_stack.push(value::from(op(a, b)));
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

        try
        {
            switch (read_byte())
            {
            case op_code::OP_CONSTANT:
                m_stack.push(read_constant());
                break;

            case op_code::OP_NIL:
                m_stack.push(value::nil());
                break;

            case op_code::OP_TRUE:
                m_stack.push(value::from(true));
                break;

            case op_code::OP_FALSE:
                m_stack.push(value::from(false));
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
                if (!m_stack.peek().is_number())
                {
                    runtime_error("Operand must be a number.");

                    return interpret_result::RUNTIME_ERROR;
                }
                m_stack.push(value::from(-m_stack.pop().as.number));
                break;

            case op_code::OP_RETURN:
                print_value(m_stack.pop());
                std::cout << '\n';
                return interpret_result::OK;
            }
        }
        catch (const interpret_result result)
        {
            return result;
        }
    }
}

void lox::vm::runtime_error(const std::string_view format, const auto&&... params)
{
    std::cerr << std::vformat(format, std::make_format_args(params...)) << '\n';

    auto instruction = m_ip - m_chunk.count() - 1;
    auto line = m_chunk.lines().get(instruction);
    std::cerr << std::format("[line {}] in script\n", line);

    m_stack.reset();
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
