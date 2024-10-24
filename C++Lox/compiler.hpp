
#pragma once

#include "common.hpp"
#include "scanner.hpp"

namespace lox
{
    void compile(lox::scanner& scanner)
    {
        int line = -1;
        for (;;) 
        {
            token token = scanner.scan_token();

            if (token.line != line) 
            {
                printf("%4d ", token.line);
                line = token.line;
            }
            else 
            {
                printf("   | ");
            }
            printf("%2d '%.*s'\n", token.type, token.length, token.start);

            if (token.type == token_type::END_OF_FILE) break;
        }
    }
}
