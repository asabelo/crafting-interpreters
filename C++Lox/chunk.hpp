
#pragma once

#include "array.hpp"
#include "common.hpp"
#include "value.hpp"

namespace lox
{
    // Bytecode instruction operation code
    enum op_code : uint8_t
    {
        OP_RETURN,
        OP_CONSTANT
    };

    struct line_info
    {
        int number = 0;
        int op_count = 0;
    };

    using line_array = array<line_info>;

    class chunk : public array<uint8_t>
    {
        line_array m_lines = {};

        value_array m_constants = {};

    public:

              line_array& lines()       { return m_lines; }
        const line_array& lines() const { return m_lines; }

              value_array& constants()       { return m_constants; }
        const value_array& constants() const { return m_constants; }

        idx_t add(elem_t element, int line)
        {
            if (auto line_count = m_lines.count(); line_count > 0)
            {
                auto& [line_num, op_count] = m_lines.get(line_count - 1);

                if (line == line_num)
                {
                    op_count++;
                }
                else
                {
                    m_lines.add({ line, 1 });
                }
            }
            else
            {
                m_lines.add({ line, 1 });
            }

            return array::add(element);
        }

        int get_line(int op_index) const
        {
            if (op_index < 0) return -1;

            int line = -1;
            int ops = 0;

            for (line_array::idx_t i = 0; i < m_lines.count() && ops <= op_index; ++i)
            {
                const auto& [new_line, op_count] = m_lines.get(i);

                line = new_line;
                ops += op_count;
            }

            return ops < op_index ? -1 : line;
        }
    };
}
