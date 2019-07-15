#ifndef GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#define GEAROENIX_CORE_SYNC_KERNEL_WORKERS_HPP
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace gearoenix::core::sync {
class Semaphore;
class KernelWorker {
private:
    typedef enum : int {
        RUNNING,
        TERMINATING,
        TERMINATED,
    } State;
    struct Worker {
        const std::vector<std::shared_ptr<Semaphore>> waits;
        const std::function<void(const unsigned int)> worker;
        const std::function<void()> boss;
        const std::vector<std::shared_ptr<Semaphore>> signals;
    };
    std::vector<std::shared_ptr<Semaphore>> signals;
    std::vector<std::thread> threads;
    std::vector<Worker> workers;
    // For each thread there is a separate mutex to donot lock all threads by one mutex
    std::vector<std::shared_ptr<std::mutex>> workers_syncers;
    State state = RUNNING;

    void thread_loop(const unsigned int);

public:
    KernelWorker();
    ~KernelWorker();
    void add_step(
        std::function<void(const unsigned int)> worker, std::function<void()> boss = [] {});
    void do_steps();
    unsigned int get_threads_count() const;
};
}
#endif