#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/skybox/gx-rnd-sky-manager.hpp"

namespace gearoenix::vulkan::skybox {
struct Manager final : render::skybox::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;

    [[nodiscard]] core::ecs::EntityPtr build(
        std::string&& name, core::ecs::Entity* parent, std::shared_ptr<render::texture::Texture>&& bound_texture, std::shared_ptr<render::mesh::Mesh>&& bound_mesh) override;
};
}
#endif
