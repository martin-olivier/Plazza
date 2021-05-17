#include <iostream>
#include "exceptions.hpp"
#include "shell.hpp"
#include "utils.hpp"
#include "pizza.hpp"
#include "logger.hpp"

void plazza::help(std::string_view bin, int exit_status)
{
    std::cout << "USAGE\n    " << bin << " c n r\n\n";
    std::cout << "DESCRIPTION\n";
    std::cout << "    c\tmultiplier for the cooking time of the pizzas\n";
    std::cout << "    n\tnumber of cooks per kitchen\n";
    std::cout << "    r\ttime in milliseconds used by the kitchen stock to replace ingredients\n";
    exit(exit_status);
}

void plazza::shell::parsing(int ac, char **av)
{
    float multiplier;
    int cooks;
    int replace_time;

    if (ac == 2 and (std::string_view(av[1]) == "-h" or std::string_view(av[1]) == "--help"))
        help(av[0], 0);
    if (ac != 4)
        help(av[0], 84);
    try {
        multiplier = std::stof(av[1]);
        cooks = std::stoi(av[2]);
        replace_time = std::stoi(av[3]);
    }
    catch (...) {
        throw plazza::bad_argument();
    }
    if (multiplier < 0 or cooks <= 0 or replace_time <= 0)
        throw plazza::bad_argument();
    m_core.start(multiplier, cooks, replace_time);
}

void plazza::shell::details() const noexcept
{
    int i = 0;
    m_core.get_reception().m_mutex.lock();
    m_core.m_db_mutex.lock();
    logger("[DETAILS]\n");
    logger("Kitchens Opened : ", m_core.get_database().size(), "\n");
    for (const auto &kitchen : m_core.get_database()) {
        logger("  Cooks Opened at kitchen ", i, " : ", static_cast<int>(kitchen->m_contains), "\n");
        i++;
    }
    i = 0;
    int j = 0;
    for (const auto &kitchen : m_core.get_database()) {
        logger("kitchen ", j, " :\n");
        for (const auto &ingredient : kitchen->m_ingredients) {
            logger("  " , mapPizzaIngredients[i], " : ", static_cast<int>(ingredient), "\n");
            i++;
        }
        i = 0;
        j++;
    }
    m_core.get_reception().m_mutex.unlock();
    m_core.m_db_mutex.unlock();
}

void plazza::shell::factory(const PizzaType &type, const PizzaSize &size)
{
    if (type == Regina)
        m_factory_buffer.push_back(std::make_shared<regina>(size));
    else if (type == Margarita)
        m_factory_buffer.push_back(std::make_shared<margarita>(size));
    else if (type == Americana)
        m_factory_buffer.push_back(std::make_shared<americana>(size));
    else if (type == Fantasia)
        m_factory_buffer.push_back(std::make_shared<fantasia>(size));
    else
        throw plazza::bad_command();
}

void plazza::shell::check_cmd(const std::string &cmd)
{
    auto parse = utils::stringToVector(cmd, ' ');
    if (parse.size() != 3)
        throw plazza::bad_command();
    if (mapPizzaType.find(parse[0]) == mapPizzaType.end())
        throw plazza::bad_command();
    if (mapPizzaSize.find(parse[1]) == mapPizzaSize.end())
        throw plazza::bad_command();
    if (parse[2].size() < 2)
        throw plazza::bad_command();
    if (parse[2][0] != 'x')
        throw plazza::bad_command();
    try {
        int num = std::stoi(parse[2].substr(1));
        if (num <= 0)
            throw plazza::bad_command();
        for (int i = 0; i < num; i++)
            factory(mapPizzaType[parse[0]], mapPizzaSize[parse[1]]);
    } catch (...) {
        throw plazza::bad_command();
    }
}

void plazza::shell::apply(const std::string &cmd)
{
    auto cmd_list = utils::stringToVector(cmd, ';');
    for (auto &cmd_part : cmd_list)
        check_cmd(cmd_part);
    for (auto &pizza : m_factory_buffer)
        m_core.add_pizza(pizza);
    m_factory_buffer.clear();
    m_core.loop();
}

plazza::shell::shell(int ac, char **av)
{
    parsing(ac, av);
    std::string input;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, input))
            break;
        if (input == "exit")
            break;
        else if (input == "clear")
            std::system("clear");
        else if (input == "details")
            details();
        else {
            try {
                apply(input);
            } catch (const plazza::bad_command &e) {
                std::cout << "Invalid command" << std::endl;
                m_factory_buffer.clear();
            }
        }
    }
}