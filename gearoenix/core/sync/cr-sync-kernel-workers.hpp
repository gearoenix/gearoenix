#ifndef GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#include "../cr-build-configuration.hpp"
#include <functional>
#include <memory>
#include <vector>

#ifndef GX_THREAD_NOT_SUPPORTED
#include <mutex>
#include <thread>

namespace gearoenix::core::sync {
class Semaphore;
}
#endif

namespace gearoenix::core::sync {
class KernelWorkers {
private:
    struct Worker {
#ifndef GX_THREAD_NOT_SUPPORTED
        const std::vector<std::shared_ptr<Semaphore>> waits;
        const std::vector<std::shared_ptr<Semaphore>> signals;
#endif
        const std::function<void()> sender;
        const std::function<void(const unsigned int)> worker;
        const std::function<void()> meanwhile;
        const std::function<void()> receiver;
    };
    std::vector<Worker> workers;

#ifndef GX_THREAD_NOT_SUPPORTED
    enum struct State : int {
        Running,
        Terminating,
        Terminated,
    };

    State state = State::Running;
    std::vector<std::shared_ptr<Semaphore>> signals;
    std::vector<std::thread> threads;
    // For each thread there is a separate mutex to do not lock all threads by one mutex
    std::vector<std::shared_ptr<std::mutex>> workers_syncers;

    void thread_loop(unsigned int) noexcept;
#endif

public:
    KernelWorkers() noexcept;
    ~KernelWorkers() noexcept;
    void add_step(
        std::function<void(const unsigned int)> worker,
        std::function<void()> receiver = [] {}) noexcept;
    void add_step(
        std::function<void()> sender,
        std::function<void(const unsigned int)> worker,
        std::function<void()> meanwhile,
        std::function<void()> receiver) noexcept;
    void do_steps() noexcept;
    [[nodiscard]] unsigned int get_threads_count() const noexcept;
};
}
#endif