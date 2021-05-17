#include "core.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

bool try_get_ingredients(std::shared_ptr<plazza::database> database, std::shared_ptr<plazza::IPizza> pizza)
{
    std::scoped_lock(database->m_mutex);
    for (const auto &ingredient : pizza->getIngredients())
        if (database->m_ingredients[static_cast<int>(ingredient)] <= 0)
            return false;
    for (const auto &ingredient : pizza->getIngredients())
        database->m_ingredients[static_cast<int>(ingredient)] -= 1;
    return true;
}

void plazza::thread_cook(std::shared_ptr<bool> alive, plazza::reception &reception, std::shared_ptr<plazza::database> database, std::shared_ptr<plazza::IPizza> pizza)
{
    auto cooking_time = static_cast<float>(pizza->getCookingTime());

    do {} while (!try_get_ingredients(database, pizza));

    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::microseconds(static_cast<int>(plazza::core::m_multiplier * 1000000 * cooking_time));
    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
    }
    reception.m_mutex.lock();
    reception.m_out_buffer.push_back(pizza);
    reception.m_mutex.unlock();
    database->m_contains--;
    *alive = false;
}

void plazza::thread_kitchen(std::shared_ptr<bool> alive, plazza::reception &reception, std::shared_ptr<plazza::database> database)
{
    thread_pool cooks;
    std::vector<std::shared_ptr<bool>> cooks_alive;
    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::seconds (5);
    while (true) {
        if (std::chrono::system_clock::now() >= deadline) {
            cooks.release();
            *alive = false;
            return;
        }
        database->m_mutex.lock();
        if (!database->m_buffer.empty()) {
            deadline = std::chrono::system_clock::now() + std::chrono::seconds (5);
            auto pizza = database->m_buffer.back();
            database->m_buffer.pop_back();
            cooks_alive.emplace_back(std::make_shared<bool>(true));
            cooks.add(thread_cook, cooks_alive.back(), std::ref(reception), database, pizza);
        }
        database->m_mutex.unlock();
        for (size_t i = 0; i < cooks_alive.size(); i++) {
            if (!*cooks_alive[i]) {
                deadline = std::chrono::system_clock::now() + std::chrono::seconds (5);
                cooks.remove(i);
                cooks_alive.erase(cooks_alive.begin() + i);
            }
        }
    }
}