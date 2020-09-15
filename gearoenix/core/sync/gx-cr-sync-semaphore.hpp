#ifndef GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#include "../gx-cr-build-configuration.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace gearoenix::core::sync {
class Semaphore {
private:
    std::mutex m;
    std::condition_variable c;
    std::atomic<int> count = 0;

public:
    explicit Semaphore(int count = 0) noexcept;
    Semaphore(Semaphore const&) = delete;
    void operator=(Semaphore const&) = delete;
    ~Semaphore() noexcept = default;
    void lock() noexcept;
    template <class Clock, class Duration>
    void lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time) noexcept;
    void release() noexcept;
};
}

template <class Clock, class Duration>
void gearoenix::core::sync::Semaphore::lock_until(
    const std::chrono::time_point<Clock, Duration>& timeout_time) noexcept
{

    std::unique_lock<std::mutex> lock(m);
    if (c.wait_until(lock, timeout_time, [this] { return count > 0; })) {
        --count;
    }
}
#endif
#endif
