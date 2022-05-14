#ifndef GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_SEMAPHORE_HPP
#include "../gx-cr-build-configuration.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include <condition_variable>
#include <mutex>

namespace gearoenix::core::sync {
struct Semaphore final {
private:
    std::mutex m;
    std::condition_variable c;
    int count = 0;

public:
    explicit Semaphore(int count = 0) noexcept;
    Semaphore(Semaphore const&) = delete;
    void operator=(Semaphore const&) = delete;
    ~Semaphore() noexcept = default;
    void lock() noexcept;
    void release() noexcept;

    template <typename Duration>
    void lock_for(const Duration& timeout_time) noexcept
    {
        std::unique_lock<std::mutex> lock(m);
        if (c.wait_for(lock, timeout_time, [this] { return count > 0; })) {
            --count;
        }
    }
};
}
#endif
#endif
