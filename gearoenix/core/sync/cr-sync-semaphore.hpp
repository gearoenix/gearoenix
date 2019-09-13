#ifndef GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#include "../cr-build-configuration.hpp"
#include <atomic>
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
    explicit Semaphore(int count = 0);
    Semaphore(Semaphore const&) = delete;
    void operator=(Semaphore const&) = delete;
    ~Semaphore() = default;
    void lock();
    void release();
};
}
#endif
