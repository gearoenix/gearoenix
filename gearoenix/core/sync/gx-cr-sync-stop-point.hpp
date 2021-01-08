#ifndef GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#define GEAROENIX_CORE_SYNC_STOP_POINT_HPP
#include "../gx-cr-build-configuration.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include <memory>
#include <mutex>

namespace gearoenix::core::sync {
struct Semaphore;
struct StopPoint {
private:
    const std::shared_ptr<Semaphore> sem;
    const int stopper;
    int counter = 0;
    std::mutex counter_lock;

public:
    explicit StopPoint(int walkers_count);
    void all_reach();
};
}
#endif
#endif