#ifndef GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_CHANNELED_WORKERS_HPP
#include "gx-cr-sync-channel.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace gearoenix::core::sync {
struct ChanneledWorkers {
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
        void kernel();
        explicit Thread(std::size_t i);
        ~Thread();
    };
    std::vector<std::unique_ptr<Thread>> threads;

public:
    ChanneledWorkers();
    ~ChanneledWorkers();

    ChanneledWorkers(const ChanneledWorkers&) = delete;
    void operator=(const ChanneledWorkers&) = delete;

    void perform(const std::function<void(std::size_t)>& job);
};
}
#endif