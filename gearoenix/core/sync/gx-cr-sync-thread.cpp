#include "gx-cr-sync-thread.hpp"
#include <thread>

std::int32_t gearoenix::core::sync::threads_count()
{
    const auto count = static_cast<std::int32_t>(std::thread::hardware_concurrency());
    return count;
}