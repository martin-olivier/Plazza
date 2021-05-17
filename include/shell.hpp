#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include "core.hpp"

namespace plazza
{
    class shell
    {
    private:
        core m_core{};
        std::vector<std::shared_ptr<IPizza>> m_factory_buffer{};
    public:
        shell(int ac, char **av);
        ~shell() = default;
        void parsing(int ac, char **av);
        void details() const noexcept;
        void apply(const std::string &cmd);
        void check_cmd(const std::string &cmd);
        void factory(const PizzaType &type, const PizzaSize &size);
    };
    void help(std::string_view bin, int exit_status);
}