#pragma once

#include <mutex>
#include <iostream>
#include <fstream>

namespace plazza
{
    inline void print()
    {
        std::cout << std::flush;
    }

    template <typename T, typename... Args>
    void print(T var1, Args ...var2)
    {
        std::cout << var1;
        std::ofstream file;
        file.open("log.txt", std::ios::app);
        file << var1;
        file.close();
        print(var2...);
    }

    template<typename T, typename ...Args>
    void logger(T type, Args ...args)
    {
        static std::mutex mtx;
        mtx.lock();
        print(type, args...);
        mtx.unlock();
    }
}