
#include "vm.hpp"

#include "compiler.hpp"
#include "debug.hpp"
#include "memory.hpp"

lox::vm::vm(lox::chunk& chunk) 
    : m_chunk{ chunk }
    , m_ip{ 0 }
    , m_stack{}
    , m_strings{}
    , m_globals{}
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
        return m_chunk.at(m_ip++);
    };

    const auto read_short = [this]()
    {
        return static_cast<uint16_t>((m_chunk.at(m_ip++).op << 8) | m_chunk.at(m_ip++).op);
    };

    const auto read_constant = [this, &read_byte]()
    {
        return m_chunk.constants().at(read_byte().op);
    };

    const auto binary_op = [this](auto operation)
    {
        const auto b = m_stack.back();
        m_stack.pop_back();

        const auto a = m_stack.back();
        m_stack.pop_back();

        if (not b.is_number() or not a.is_number())
        {
            m_stack.push_back(a);
            m_stack.push_back(b);

            runtime_error("Operands must be numbers.");
            throw interpret_result::RUNTIME_ERROR;
        }

        m_stack.push_back(value::from(operation(a.as_number(), b.as_number())));
    };

#ifdef _DEBUG
    std::cout << "\n== trace ==";
#endif // _DEBUG

    while (true)
    {
#ifdef _DEBUG
        std::cout << "          ";

        for (std::vector<value>::size_type i = 0; i < m_stack.size(); ++i)
        {
            std::cout << "[ ";

            m_stack.at(i).print();

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
                m_stack.push_back(read_constant());
                break;

            case op_code::OP_NIL:
                m_stack.push_back(value::nil());
                break;

            case op_code::OP_TRUE:
                m_stack.push_back(value::from(true));
                break;

            case op_code::OP_FALSE:
                m_stack.push_back(value::from(false));
                break;

            case op_code::OP_POP:
                m_stack.pop_back();
                break;

            case op_code::OP_GET_LOCAL:
                {
                    auto slot = read_byte();
                    m_stack.push_back(m_stack.at(slot.op));
                }
                break;

            case op_code::OP_SET_LOCAL:
                {
                    auto slot = read_byte();
                    m_stack.at(slot.op) = m_stack.back();
                }
                break;

            case op_code::OP_GET_GLOBAL:
                {
                    auto name = read_constant().as_string();

                    if (not m_globals.contains(name))
                    {
                        runtime_error("Undefined variable '{0}'.", name->chars());
                        return interpret_result::RUNTIME_ERROR;
                    }

                    m_stack.push_back(m_globals[name]);
                }
                break;

            case op_code::OP_DEFINE_GLOBAL:
                {
                    auto name = read_constant().as_string();
                    m_globals[name] = m_stack.back();
                    m_stack.pop_back();
                }
                break;

            case op_code::OP_SET_GLOBAL:
                {
                    auto name = read_constant().as_string();

                    if (!m_globals.contains(name))
                    {
                        runtime_error("Undefined variable '{0}'.", name->chars());
                        return interpret_result::RUNTIME_ERROR;
                    }

                    m_globals[name] = m_stack.back();
                }
                break;

            case op_code::OP_EQUAL:
                {
                    const auto b = m_stack.back();
                    m_stack.pop_back();

                    const auto a = m_stack.back();
                    m_stack.pop_back();

                    m_stack.push_back(value::from(std::equal_to<value>{}(a, b)));
                }
                break;

            case op_code::OP_GREATER:
                binary_op(std::greater{});
                break;

            case op_code::OP_LESS:
                binary_op(std::less{});
                break;

            case op_code::OP_ADD:
                {
                    const auto b = m_stack.back();
                    m_stack.pop_back();

                    const auto a = m_stack.back();
                    m_stack.pop_back();

                    if (b.is_string() and a.is_string())
                    {
                        m_stack.push_back(a);
                        m_stack.push_back(b);

                        concatenate();
                    }
                    else if (b.is_number() and a.is_number())
                    {
                        m_stack.push_back(a);
                        m_stack.push_back(b);

                        binary_op(std::plus{});
                    }
                    else
                    {
                        m_stack.push_back(a);
                        m_stack.push_back(b);

                        runtime_error("Operands must be two numbers or two strings.");
                        return interpret_result::RUNTIME_ERROR;
                    }
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
                {
                    const auto a = m_stack.back();
                    m_stack.pop_back();
                    m_stack.push_back(value::from(a.is_falsey()));
                }
                break;

            case op_code::OP_NEGATE:
                {
                    const auto a = m_stack.back();

                    if (not a.is_number())
                    {
                        runtime_error("Operand must be a number.");
                        return interpret_result::RUNTIME_ERROR;
                    }

                    m_stack.pop_back();
                    m_stack.push_back(value::from(-a.as_number()));
                }
                break;

            case op_code::OP_PRINT:
                m_stack.back().print();
                m_stack.pop_back();
                std::cout << '\n';
                break;

            case op_code::OP_JUMP:
                {
                    auto offset = static_cast<std::vector<op_info>::size_type>(read_short());
                    m_ip += offset;
                }
                break;

            case op_code::OP_JUMP_IF_FALSE:
                {
                    auto offset = static_cast<std::vector<op_info>::size_type>(read_short());

                    if (m_stack.back().is_falsey())
                    {
                        m_ip += offset;
                    }
                }
                break;

            case op_code::OP_LOOP:
                {
                    auto offset = static_cast<std::vector<op_info>::size_type>(read_short());
                    m_ip -= offset;
                }
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
    const auto b = m_stack.back().as_string();
    m_stack.pop_back();

    const auto a = m_stack.back().as_string();
    m_stack.pop_back();

    a->concat(*b);

    m_stack.push_back(value::from(a));
}

void lox::vm::runtime_error(const std::string_view format, const auto&&... params)
{
    std::cerr << std::vformat(format, std::make_format_args(params...)) << '\n';

    auto instruction = m_chunk.size() - m_ip - 1;
    auto line = m_chunk.at(instruction).line;
    std::cerr << std::format("[line {}] in script\n", line);

    while (not m_stack.empty()) m_stack.pop_back();
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
