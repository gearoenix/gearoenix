#ifndef GEAROENIX_CORE_SYNC_CHANNEL_HPP
#define GEAROENIX_CORE_SYNC_CHANNEL_HPP
#include "../gx-cr-build-configuration.hpp"
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
    Channel() = default;

    Channel(const Channel&) = delete;
    void operator=(const Channel&) = delete;

    [[nodiscard]] std::size_t get_pending() const
    {
        return data.size();
    }

    void send(const T& t)
    {
        std::lock_guard<std::mutex> _l(m);
        data.push(t);
        s.release();
    }

    T receive()
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
