#pragma once
#include "../gx-cr-build-configuration.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace gearoenix::core::sync {
struct Semaphore;
}

namespace gearoenix::core::sync {
struct KernelWorkers final {
private:
    struct Worker final {
        const std::vector<std::shared_ptr<Semaphore>> waits;
        const std::vector<std::shared_ptr<Semaphore>> signals;
        const std::function<void()> sender;
        const std::function<void(const unsigned int)> worker;
        const std::function<void()> meanwhile;
        const std::function<void()> receiver;
    };
    std::vector<Worker> workers;

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

    void thread_loop(unsigned int);

public:
    KernelWorkers();
    ~KernelWorkers();
    void add_step(std::function<void(unsigned int)> worker, std::function<void()> receiver = [] { });
    void add_step(std::function<void()> sender, std::function<void(unsigned int)> worker, std::function<void()> meanwhile, std::function<void()> receiver);
    void do_steps();
    [[nodiscard]] unsigned int get_threads_count() const;
};
}