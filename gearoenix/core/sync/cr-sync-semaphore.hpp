#ifndef GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#define GEAROENIX_CORE_SYNC_QUEUED_SEMAPHORE_HPP
#include "../cr-build-configuration.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>

namespace gearoenix::core::sync {
class Semaphore {
private:
    std::mutex m;
    std::condition_variable c;
    std::atomic<int> count = 0;

public:
    Semaphore(int count = 0);
    ~Semaphore();
    void lock();
    void release();
};
}
#endif
