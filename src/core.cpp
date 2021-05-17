#include "core.hpp"
#include "logger.hpp"

float plazza::core::m_multiplier = 0;
int plazza::core::m_cooks = 0;
int plazza::core::m_replace_time = 0;

void plazza::core::add_pizza(const std::shared_ptr<IPizza> &pizza)
{
    m_reception.m_mutex.lock();
    m_reception.m_in_buffer.push_back(pizza);
    m_reception.m_mutex.unlock();
}

void plazza::core::start(float multiplier, int cooks, int replace_time)
{
    m_multiplier = multiplier;
    m_cooks = cooks;
    m_replace_time = replace_time;

    std::thread thread_killer(kill_terminated_threads, std::ref(m_reception), std::ref(m_pool), std::ref(m_database), std::ref(m_db_mutex), std::ref(m_kitchens_alive));
    thread_killer.detach();
    std::thread ingredients_thread(update_ingredients, std::ref(m_database), std::ref(m_db_mutex));
    ingredients_thread.detach();
}

inline int plazza::core::find_lower(std::vector<std::shared_ptr<plazza::database>> &database)
{
    int lower = -1;
    bool ok = false;

    for (const auto &kitchen : database) {
        if (kitchen->m_contains < plazza::core::m_cooks) {
            lower = kitchen->m_contains;
            ok = true;
            break;
        }
    }
    if (!ok)
        return -1;
    for (const auto &kitchen : database) {
        if (kitchen->m_contains < lower)
            lower = kitchen->m_contains;
    }
    return lower;
}

void plazza::core::loop()
{
    m_reception.m_mutex.lock();
    for (auto &in : m_reception.m_in_buffer)
        logger("[RECEPTION] New order : ", revMapPizzaType[in->getType()], " ", revMapPizzaSize[in->getSize()], "\n");
    for (auto &pizza : m_reception.m_in_buffer) {
        bool done = false;
        if (m_database.empty()) {
            m_database.push_back(std::make_shared<database>());
            m_kitchens_alive.emplace_back(std::make_shared<bool>(true));
            m_pool.add(thread_kitchen, m_kitchens_alive.back(),std::ref(m_reception), m_database.back());
        }
        for (auto &kitchen : m_database) {
            if (kitchen->m_contains < m_cooks and kitchen->m_contains == find_lower(m_database)) {
                kitchen->push(pizza);
                done = true;
                break;
            }
        }
        if (!done) {
            m_database.push_back(std::make_shared<database>());
            m_kitchens_alive.emplace_back(std::make_shared<bool>(true));
            m_pool.add(thread_kitchen, m_kitchens_alive.back(), std::ref(m_reception), m_database.back());
            m_database.back()->push(pizza);
        }
    }
    m_reception.m_in_buffer.clear();
    m_reception.m_mutex.unlock();
}

[[noreturn]] void plazza::core::kill_terminated_threads(plazza::reception &reception, thread_pool &pool,
                                           std::vector<std::shared_ptr<plazza::database>> &database,
                                           std::mutex &db_mutex,
                                           std::vector<std::shared_ptr<bool>> &kitchens_alive)
{
    using namespace std::chrono_literals;
    while (true) {
        std::this_thread::sleep_for(0.1s);
        reception.m_mutex.lock();
        db_mutex.lock();
        for (size_t i = 0; i < kitchens_alive.size(); i++) {
            if (!*kitchens_alive[i]) {
                pool.remove(i);
                kitchens_alive.erase(kitchens_alive.begin() + i);
                database.erase(database.begin() + i);
                logger("[THREAD] a kitchen closed (thread killed)\n");
                i = 0;
            }
        }
        for (auto &out : reception.m_out_buffer)
            logger("[RECEPTION] Order ready : ", revMapPizzaType[out->getType()], " ", revMapPizzaSize[out->getSize()], "\n");
        reception.m_out_buffer.clear();
        reception.m_mutex.unlock();
        db_mutex.unlock();
    }
}

[[noreturn]] void plazza::core::update_ingredients(std::vector<std::shared_ptr<database>> &database, std::mutex &db_mutex)
{
    while (true) {
        std::chrono::milliseconds duration(plazza::core::m_replace_time);
        std::this_thread::sleep_for(duration);
        for (auto & db : database) {
            db->m_mutex.lock();
            db_mutex.lock();
            for (size_t i = 0; i < 9; i++)
                db->m_ingredients[i] += 1;
            db->m_mutex.unlock();
            db_mutex.unlock();
        }
        logger("[THREAD] ingredients updated\n");
    }
}