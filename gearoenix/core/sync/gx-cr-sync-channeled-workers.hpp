#pragma once
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
        const std::uint32_t thread_index;
        bool running = true;
        std::mutex pending_lock;
        std::vector<std::function<void(std::uint32_t)>> pending;
        std::vector<std::function<void(std::uint32_t)>> underprocess;
        std::uint32_t underprocess_index = 0;
        Semaphore signal;
        std::thread thread;
        void kernel();
        explicit Thread(std::uint32_t i);
        ~Thread();
    };
    std::vector<std::unique_ptr<Thread>> threads;

public:
    ChanneledWorkers();
    ~ChanneledWorkers();

    ChanneledWorkers(const ChanneledWorkers&) = delete;
    void operator=(const ChanneledWorkers&) = delete;

    void perform(const std::function<void(std::uint32_t)>& job);
};
}