#ifndef GEAROENIX_RENDER_SCENE_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_SCENE_HPP
#include "../../core/ecs/gx-cr-ecs-component.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include <vector>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::gltf {
struct Loader;
}

namespace gearoenix::render::scene {
struct Scene final : public core::ecs::Component {
    GX_GET_CREF_PRV(std::vector<core::ecs::Entity::id_t>, entities)

public:
    Scene(
        engine::Engine* e,
        std::size_t scene_index,
        gltf::Loader& loader,
        const core::sync::EndCallerIgnored& c = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    ~Scene() noexcept final;
    Scene(Scene&&) noexcept;
    void update() noexcept;
};
}
#endif
