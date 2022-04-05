#ifndef GEAROENIX_RENDER_GLTF_LOADER_HPP
#define GEAROENIX_RENDER_GLTF_LOADER_HPP
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/sync/gx-cr-sync-end-caller.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render {
[[nodiscard]] std::vector<core::ecs::Entity::id_t> load_gltf(
    engine::Engine& e,
    const platform::stream::Path& file,
    const core::sync::EndCallerIgnored& end_callback = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
}

#endif