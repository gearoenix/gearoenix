#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vertex.hpp"
#include <vector>

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::buffer {
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
    [[nodiscard]] virtual std::shared_ptr<Uniform> create_uniform(std::size_t size, std::size_t frame_number) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<Static> create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<Static> create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept = 0;
};
}
#endif
