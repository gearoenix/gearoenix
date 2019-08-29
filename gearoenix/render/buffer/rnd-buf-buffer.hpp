#ifndef GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#define GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#include <vector>
#include "../../core/cr-static.hpp"
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace buffer {
    class Buffer {
    protected:
        engine::Engine* const e;
        const std::size_t size;

        Buffer(const std::size_t size, engine::Engine* const e) noexcept
            : e(e)
            , size(size)
        {
        }

    public:
        virtual ~Buffer() noexcept = default;
		GX_GETTER_BUILDER_V(size)
    };
}
}
#endif
