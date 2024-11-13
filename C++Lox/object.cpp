
#include "object.hpp"

#include "memory.hpp"
#include "vm.hpp"

template <typename T = lox::obj>
static T* allocate_obj(lox::obj_type type)
{
    auto* obj = static_cast<lox::obj*>(lox::allocate<T>());

    obj->type = type;
    obj->next = lox::vm::objects;
    lox::vm::objects = obj;

    return static_cast<T*>(obj);
}

static lox::obj_string* allocate_string(char* chars, std::size_t length)
{
    auto* string = allocate_obj<lox::obj_string>(lox::obj_type::STRING);

    string->length = length;
    string->chars = chars;

    return string;
}

lox::obj_string* lox::take_string(char* chars, std::size_t length)
{
    return allocate_string(chars, length);
}

lox::obj_string* lox::copy_string(const std::string_view text)
{
    auto* chars = allocate<char>(text.length() + 1);

    std::copy(text.cbegin(), text.cend(), chars);

    chars[text.length()] = '\0';

    return allocate_string(chars, text.length());
}

void lox::print_object(value value)
{
    switch (value.as.object->type)
    {
    case lox::obj_type::STRING:
        std::cout << static_cast<obj_string*>(value.as.object)->chars;
        break;
    }
}
