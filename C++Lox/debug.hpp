
#pragma once

#include "chunk.hpp"

namespace lox
{
    void disassemble_chunk(const chunk& chunk, const char* name);

    int disassemble_instruction(const chunk& chunk, int offset);
}
