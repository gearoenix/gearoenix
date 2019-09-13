#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include <vector>

namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace buffer {
    class Static;
    class Uniform;
    /// All of the functions must be internally synchronized
    class Manager {
    protected:
        engine::Engine* const e;
        explicit Manager(engine::Engine* e) noexcept
            : e(e)
        {
        }

    public:
        virtual ~Manager() noexcept = default;
        virtual Uniform* create_uniform(std::size_t size) noexcept = 0;
        virtual Static* create_static(std::vector<math::BasicVertex> vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept = 0;
        virtual Static* create_static(std::vector<std::uint32_t> indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept = 0;
    };
}
}
#endif
