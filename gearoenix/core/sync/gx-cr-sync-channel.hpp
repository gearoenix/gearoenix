#ifndef GEAROENIX_CORE_SYNC_CHANNEL_HPP
#define GEAROENIX_CORE_SYNC_CHANNEL_HPP
#include "../gx-cr-build-configuration.hpp"
#ifndef GX_THREAD_NOT_SUPPORTED
#include "../gx-cr-static.hpp"
#include "gx-cr-sync-semaphore.hpp"
#include <queue>

namespace gearoenix::core::sync {
template <typename T>
struct Channel {
    GX_GET_CREF_PRV(std::queue<T>, data)
private:
    std::mutex m;
    Semaphore s;

public:
    Channel() noexcept = default;

    Channel(const Channel&) noexcept = delete;
    void operator=(const Channel&) noexcept = delete;

    [[nodiscard]] std::size_t get_pending() const noexcept
    {
        return data.size();
    }

    void send(const T& t) noexcept
    {
        std::lock_guard<std::mutex> _l(m);
        data.push(t);
        s.release();
    }

    T receive() noexcept
    {
        s.lock();
        std::lock_guard<std::mutex> _l(m);
        const T t = data.front();
        data.pop();
        return t;
    }
};
}
#endif
#endif
