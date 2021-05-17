#include <iostream>
#include "exceptions.hpp"
#include "shell.hpp"

int main(int ac, char **av)
{
    try {
        plazza::shell app(ac, av);
    }
    catch (const plazza::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    catch (...) {
        return 84;
    }
    return EXIT_SUCCESS;
}