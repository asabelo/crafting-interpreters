
#pragma once

#include "scanner.hpp"

namespace lox
{
    class parser
    {
        scanner m_scanner;

        token m_current;
        token m_previous;

        bool m_had_error;
        bool m_panic_mode;

    public:

        parser(const std::string_view source);

        token current() const;

        token previous() const;

        void advance();

        void consume(const token_type type, const std::string_view message);

        bool check(const token_type type) const;

        bool match(const token_type type);

        void error_at_current(const std::string_view message);

        void error(const std::string_view message);

        void error_at(const token& token, const std::string_view message);

        bool had_error() const;
    };
}
