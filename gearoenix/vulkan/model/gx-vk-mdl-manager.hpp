#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::vulkan::model {
struct Manager final : render::model::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;

    [[nodiscard]] core::ecs::EntityPtr build(
        std::string&& name,
        core::ecs::Entity* parent,
        render::model::meshes_set_t&& meshes,
        bool is_transformable,
        std::shared_ptr<physics::animation::Armature>&& armature) override;
};
}
#endif