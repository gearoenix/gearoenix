#ifndef GEAROENIX_RENDER_GLTF_LOADER_HPP
#define GEAROENIX_RENDER_GLTF_LOADER_HPP
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/sync/gx-cr-sync-end-caller.hpp"
#include <memory>
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render {
[[nodiscard]] std::vector<std::shared_ptr<scene::Builder>> load_gltf(
    engine::Engine& e,
    const platform::stream::Path& file,
    const core::sync::EndCaller& end_callback) noexcept;
}

#endif