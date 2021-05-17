#pragma once

namespace plazza
{
    class thread_pool
    {
    private:
        std::vector<std::thread> m_threads{};
    public:
        thread_pool() = default;
        thread_pool(const thread_pool&) = delete;
        thread_pool(thread_pool&&) = delete;
        thread_pool &operator=(const thread_pool&) = delete;
        thread_pool &operator=(thread_pool&&) = delete;
        ~thread_pool()
        {
            release();
        };
        template<typename Function, typename ...Args>
        void add(Function &&fn, Args &&...args)
        {
            m_threads.emplace_back(std::forward<Function>(fn), std::forward<Args>(args)...);
        }
        void remove(size_t position)
        {
            m_threads[position].join();
            m_threads.erase(m_threads.begin() + position);
        }
        void release()
        {
            for (auto &thread : m_threads)
                thread.join();
            m_threads.clear();
        }
    };
}