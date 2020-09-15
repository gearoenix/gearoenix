#ifndef GEAROENIX_RENDER_BUFFER_STATIC_HPP
#define GEAROENIX_RENDER_BUFFER_STATIC_HPP
#include "gx-rnd-buf-buffer.hpp"

namespace gearoenix::render::buffer {
class Static : public Buffer {
protected:
    Static(const unsigned int size, engine::Engine* const e) noexcept
        : Buffer(size, e)
    {
    }

public:
    virtual ~Static() noexcept = default;
};
}
#endif
