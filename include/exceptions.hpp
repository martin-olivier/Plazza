#pragma once

#include <exception>
#include <string>
#include <utility>

namespace plazza
{
    class exception : public std::exception
    {
    protected:
        const std::string m_error;
    public:
        explicit exception(std::string error) : m_error(std::move(error)) {};
        [[nodiscard]] const char *what() const noexcept override {return m_error.c_str();};
    };

    class bad_argument : public exception
    {
    public:
        bad_argument() : exception("plazza : bad argument") {};
        [[nodiscard]] const char *what() const noexcept override {return m_error.c_str();};
    };

    class bad_command : public exception
    {
    public:
        bad_command() : exception("plazza : bad command") {};
        [[nodiscard]] const char *what() const noexcept override {return m_error.c_str();};
    };
}