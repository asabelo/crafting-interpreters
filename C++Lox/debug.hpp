
#pragma once

#include "chunk.hpp"

namespace lox
{
    void disassemble_chunk(const chunk& chunk, const std::string& name);

    chunk::size_type disassemble_instruction(const chunk& chunk, chunk::size_type offset);
}
