
#pragma once

#include "chunk.hpp"

namespace lox
{
    void disassemble_chunk(const chunk& chunk, const std::string& name);

    chunk::idx_t disassemble_instruction(const chunk& chunk, chunk::idx_t offset);
}
