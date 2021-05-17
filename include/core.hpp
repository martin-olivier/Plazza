#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <array>
#include "pizza.hpp"
#include "thread_pool.hpp"

namespace plazza
{
    class reception
    {
    public:
        reception() = default;
        ~reception() = default;
        mutable std::mutex m_mutex;
        std::vector<std::shared_ptr<IPizza>> m_in_buffer{};
        std::vector<std::shared_ptr<IPizza>> m_out_buffer{};
    };

    class database
    {
    public:
        database() = default;
        ~database() = default;
        void push(const std::shared_ptr<IPizza> &pizza)
        {
            m_mutex.lock();
            m_buffer.push_back(pizza);
            m_mutex.unlock();
            m_contains++;
        };
        std::shared_ptr<IPizza> pop()
        {
            m_mutex.lock();
            if (m_buffer.empty())
                return nullptr;
            auto save = m_buffer.back();
            m_buffer.pop_back();
            m_mutex.unlock();
            return save;
        };
        std::atomic<int> m_contains{};
        mutable std::mutex m_mutex{};
        std::vector<std::shared_ptr<IPizza>> m_buffer{};
        std::array<std::atomic<int>, 9> m_ingredients{9, 9, 9, 9, 9, 9, 9, 9, 9};
    };

    class core
    {
    private:
        reception m_reception{};
        thread_pool m_pool{};
        std::vector<std::shared_ptr<bool>> m_kitchens_alive{};
        std::vector<std::shared_ptr<database>> m_database{};
    public:
        core() = default;
        ~core() = default;
        void add_pizza(const std::shared_ptr<IPizza> &pizza);
        void start(float multiplier, int cooks, int replace_time);
        void loop();

        [[nodiscard]] inline const auto &get_reception() const noexcept {return m_reception;};
        [[nodiscard]] inline const auto &get_pool() const noexcept {return m_pool;};
        [[nodiscard]] inline const auto &get_kitchens_alive() const noexcept {return m_kitchens_alive;};
        [[nodiscard]] inline const auto &get_database() const noexcept {return m_database;};

        [[noreturn]] static void kill_terminated_threads(reception &reception, thread_pool &pool, std::vector<std::shared_ptr<plazza::database>> &database, std::mutex &db_mutex, std::vector<std::shared_ptr<bool>> &kitchens_alive);
        [[noreturn]] static void update_ingredients(std::vector<std::shared_ptr<database>> &m_database, std::mutex &db_mutex);
        static int find_lower(std::vector<std::shared_ptr<plazza::database>> &database);

        static float m_multiplier;
        static int m_cooks;
        static int m_replace_time;

        mutable std::mutex m_db_mutex{};
    };

    void thread_kitchen(std::shared_ptr<bool> alive, plazza::reception &reception, std::shared_ptr<plazza::database> database);
    void thread_cook(std::shared_ptr<bool> alive, plazza::reception &reception, std::shared_ptr<plazza::database> database, std::shared_ptr<plazza::IPizza> pizza);
}