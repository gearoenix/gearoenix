#ifndef GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#define GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#include "../../core/cr-static.hpp"
#include <vector>
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace buffer {
    class Buffer {
        GX_GET_AVAL_PRT(std::size_t, buffer_size, 0)
    protected:
        engine::Engine* const e;

        Buffer(const std::size_t size, engine::Engine* const e) noexcept
            : buffer_size(size)
            , e(e)
        {
        }

    public:
        virtual ~Buffer() noexcept = default;
    };
}
}
#endif
