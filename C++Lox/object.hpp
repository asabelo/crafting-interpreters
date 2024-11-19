
#pragma once

#include "common.hpp"

namespace lox
{
    class value;

    enum class obj_type
    {
        STRING
    };

    class obj
    {
    protected:

        obj_type m_type;

        obj(obj_type type);

    public:

        obj_type type() const;

        virtual bool equals(const obj&) const = 0;

        virtual void print() const = 0;
    };

    class obj_string final : public obj
    {
        std::size_t m_length = 0;

        std::unique_ptr<char[]> m_chars = nullptr;

    public:

        obj_string(std::string_view text);

        obj_string(const obj_string& other);

        obj_string& operator=(obj_string other);

        obj_string(obj_string&& other) noexcept;

        obj_string& operator=(obj_string&& other) noexcept;

        std::size_t length() const;

        void concat(const obj_string& other);

        void print() const final;

        bool equals(const obj& other) const final;
    };
}
