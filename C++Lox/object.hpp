
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

        virtual void print() const = 0;

        template <typename T> friend struct std::equal_to;
    };

    class obj_string final : public obj
    {
        std::size_t m_length = 0;

        std::unique_ptr<char[]> m_chars = nullptr;

    public:

        obj_string(std::string_view text);

        obj_string(const obj_string& other) = delete;

        obj_string& operator=(obj_string other) = delete;

        obj_string(obj_string&& other) noexcept = delete;

        obj_string& operator=(obj_string&& other) noexcept = delete;

        std::size_t length() const;

        void concat(const obj_string& other);

        void print() const final;

        template <typename T> friend struct std::equal_to;

        template <typename T> friend struct std::hash;
    };
}

namespace std
{
    template <>
    struct std::equal_to<lox::obj_string>
    {
        bool operator()(const lox::obj_string& lhs, const lox::obj_string& rhs) const
        {
            return &lhs == &rhs;
        }
    };

    template <>
    struct std::equal_to<lox::obj>
    {
        bool operator()(const lox::obj& lhs, const lox::obj& rhs) const
        {
            lox::obj_type type;

            if ((type = lhs.type()) != rhs.type()) return false;

            switch (type)
            {
            case lox::obj_type::STRING:
                return std::equal_to<lox::obj_string>{}
                (
                    static_cast<const lox::obj_string&>(lhs),
                    static_cast<const lox::obj_string&>(rhs)
                );
            }

            return false;
        }
    };

    template <>
    struct std::hash<lox::obj_string>
    {
        std::size_t operator()(const lox::obj_string& string) const
        {
            std::size_t hash = 2166136261u;

            for (std::size_t i = 0; i < string.m_length; ++i)
            {
                hash ^= static_cast<std::size_t>(std::bit_cast<uint8_t>(string.m_chars[i]));
                hash *= 16777619u;
            }

            return hash;
        }
    };
}
