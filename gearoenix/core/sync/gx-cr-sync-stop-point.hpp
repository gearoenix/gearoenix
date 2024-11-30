#pragma once
#include "../gx-cr-build-configuration.hpp"
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