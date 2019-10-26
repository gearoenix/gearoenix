#ifndef GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#include "cr-sync-channel.hpp"
#include <functional>
#include <thread>

namespace gearoenix::core::sync {
class ChanneledWorkers {
private:
    struct Thread {
        const std::size_t i;
        Channel<std::function<void(std::size_t)>> c;
        Semaphore* const s;
        std::thread t;
        void kernel() noexcept;
        Thread(std::size_t i, Semaphore* s) noexcept;
    };
    std::vector<Thread> threads;
    Semaphore s;
    std::atomic<std::size_t> jobs_count = 0;

public:
    ChanneledWorkers() noexcept;

    ChanneledWorkers(const ChanneledWorkers&) noexcept = delete;
    void operator=(const ChanneledWorkers&) noexcept = delete;

    void perform(const std::function<void(std::size_t)>& job) noexcept;
    void wait() noexcept;
};
}
#endif
