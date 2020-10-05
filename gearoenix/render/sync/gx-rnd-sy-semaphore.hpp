#ifndef GEAROENIX_RENDER_SYNC_SEMAPHORE_HPP
#define GEAROENIX_RENDER_SYNC_SEMAPHORE_HPP

namespace gearoenix::render::sync {
class Semaphore {
public:
    virtual ~Semaphore() noexcept = default;
};
}

#endif