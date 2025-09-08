#pragma once
#include <condition_variable>
#include <mutex>

namespace gearoenix::core::sync {
struct Semaphore final {
private:
    std::mutex m {};
    std::condition_variable c {};
    int count = 0;

public:
    explicit Semaphore(int count = 0);

    Semaphore(Semaphore const&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(Semaphore const&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    ~Semaphore() = default;
    void lock();
    void release();

    template <typename Duration>
    void lock_for(const Duration& timeout_time)
    {
        std::unique_lock lock(m);
        if (c.wait_for(lock, timeout_time, [this] { return count > 0; })) {
            --count;
        }
    }
};
}