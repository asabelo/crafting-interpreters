
#pragma once

#include "common.hpp"
#include "scanner.hpp"

namespace lox
{
    void compile(lox::scanner& scanner)
    {
        int line = -1;

        while (true)
        {
            const auto token = scanner.scan_token();

            if (token.line != line)
            {
                std::cout << std::format("{:4} ", token.line);
                line = token.line;
            }
            else 
            {
                std::cout << "   | ";
            }

            std::cout << std::format("{:2} '{}'", static_cast<int>(token.type), token.text) << '\n';

            if (token.type == token_type::END_OF_FILE) break;
        }
    }
}
