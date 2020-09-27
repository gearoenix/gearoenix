#ifndef GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#define GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#include "../../core/gx-cr-static.hpp"
#include <vector>

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::buffer {
class Buffer {
    GX_GET_CVAL_PRT(std::size_t, size)
    GX_GET_PTRC_PRT(engine::Engine, e)
protected:
    Buffer(const std::size_t size, engine::Engine* const e) noexcept
        : size(size)
        , e(e)
    {
    }

public:
    virtual ~Buffer() noexcept = default;
};
}
#endif
