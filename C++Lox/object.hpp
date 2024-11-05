
#pragma once

#include "common.hpp"

namespace lox
{
    struct value;

    enum class obj_type
    {
        STRING
    };

    struct obj
    {
        obj_type type;
        obj* next = nullptr;
    };

    struct obj_string : public obj
    {
        std::size_t length;
        char* chars;
    };

    obj_string* take_string(char* chars, std::size_t length);
    obj_string* copy_string(const std::string_view text);

    void print_object(value value);
}
