
#include "vm.hpp"

#include "compiler.hpp"
#include "debug.hpp"
#include "memory.hpp"

lox::vm::vm(lox::chunk& chunk) 
    : m_chunk{ chunk }
    , m_ip{ 0 }
    , m_stack{}
    , m_strings{}
{
}

lox::vm::~vm()
{
    // TODO :/ ?

    auto* obj = vm::objects;

    //while (obj)
    //{
    //    auto* next = obj->next;

    //}
}

lox::obj* lox::vm::objects = nullptr;

lox::interpret_result lox::vm::run()
{
    const auto read_byte = [this]()
    {
        return m_chunk.get(m_ip++);
    };

    const auto read_constant = [this, &read_byte]()
    {
        return m_chunk.constants().get(read_byte().op);
    };

    const auto binary_op = [this](auto operation)
    {
        if (!m_stack.peek(0).is_number() || !m_stack.peek(1).is_number())
        {
            runtime_error("Operands must be numbers.");
            throw interpret_result::RUNTIME_ERROR;
        }
        const auto b = m_stack.pop().as_number();
        const auto a = m_stack.pop().as_number();
        m_stack.push(value::from(operation(a, b)));
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

            m_stack.get(i).print();

            std::cout << " ]";
        }

        std::cout << '\n';

        disassemble_instruction(m_chunk, m_ip);
#endif // DEBUG

        try
        {
            switch (read_byte().op)
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

            case op_code::OP_EQUAL:
                {
                    const auto a = m_stack.pop();
                    const auto b = m_stack.pop();
                    m_stack.push(value::from(std::equal_to<value>{}(a, b)));
                }
                break;

            case op_code::OP_GREATER:
                binary_op(std::greater{});
                break;

            case op_code::OP_LESS:
                binary_op(std::less{});
                break;

            case op_code::OP_ADD:
                if (m_stack.peek(0).is_string() && m_stack.peek(1).is_string())
                {
                    concatenate();
                }
                else if (m_stack.peek(0).is_number() && m_stack.peek(1).is_number())
                {
                    binary_op(std::plus{});
                }
                else
                {
                    runtime_error("Operands must be two numbers or two strings.");

                    return interpret_result::RUNTIME_ERROR;
                }
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

            case op_code::OP_NOT:
                m_stack.push(value::from(m_stack.pop().is_falsey()));
                break;

            case op_code::OP_NEGATE:
                if (!m_stack.peek().is_number())
                {
                    runtime_error("Operand must be a number.");

                    return interpret_result::RUNTIME_ERROR;
                }
                m_stack.push(value::from(-m_stack.pop().as_number()));
                break;

            case op_code::OP_PRINT:
                m_stack.pop().print();
                std::cout << '\n';
                break;

            case op_code::OP_RETURN:
                return interpret_result::OK;
            }
        }
        catch (const interpret_result result)
        {
            return result;
        }
    }
}

void lox::vm::concatenate()
{
    auto b = std::static_pointer_cast<obj_string>(m_stack.pop().as_object());
    auto a = std::static_pointer_cast<obj_string>(m_stack.pop().as_object());

    a->concat(*b);

    m_stack.push(value::from(a));
}

void lox::vm::runtime_error(const std::string_view format, const auto&&... params)
{
    std::cerr << std::vformat(format, std::make_format_args(params...)) << '\n';

    auto instruction = m_ip - m_chunk.count() - 1;
    auto line = m_chunk.get(instruction).line;
    std::cerr << std::format("[line {}] in script\n", line);

    m_stack.reset();
}

lox::interpret_result lox::vm::interpret(const std::string_view source)
{
    lox::compiler compiler{ source, m_chunk, m_strings };

    if (!compiler.compile())
    {
        return interpret_result::COMPILE_ERROR;
    }

    auto result = run();

    return result;
}
