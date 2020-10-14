#ifndef GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#include "gx-cr-sync-channel.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace gearoenix::core::sync {
class ChanneledWorkers {
private:
    struct Thread {
        const std::size_t thread_index;
        bool running = true;
        std::mutex pending_lock;
        std::vector<std::function<void(std::size_t)>> pending;
        std::vector<std::function<void(std::size_t)>> underprocess;
        std::size_t underprocess_index = 0;
        Semaphore signal;
        std::thread thread;
        void kernel() noexcept;
        explicit Thread(std::size_t i) noexcept;
        ~Thread() noexcept;
    };
    std::vector<std::unique_ptr<Thread>> threads;

public:
    ChanneledWorkers() noexcept;
    ~ChanneledWorkers() noexcept;

    ChanneledWorkers(const ChanneledWorkers&) noexcept = delete;
    void operator=(const ChanneledWorkers&) noexcept = delete;

    void perform(const std::function<void(std::size_t)>& job) noexcept;
};
}
#endif
#endif