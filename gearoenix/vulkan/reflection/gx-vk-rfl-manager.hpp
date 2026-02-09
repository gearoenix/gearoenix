#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

namespace gearoenix::vulkan::reflection {
struct Manager final : render::reflection::Manager, core::Singleton<Manager> {
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataReflectionProbe>, uniform_indexer);

    void update() override;

public:
    Manager();
    ~Manager() override;
    [[nodiscard]] core::ecs::EntityPtr build_baked(
        std::string&& name,
        core::ecs::Entity* parent,
        std::shared_ptr<render::texture::TextureCube>&& irradiance,
        std::shared_ptr<render::texture::TextureCube>&& radiance,
        const math::Aabb3<double>& include_box) override;
    void build_runtime(
        std::string&& name, core::ecs::Entity* parent,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint32_t environment_resolution,
        std::uint32_t irradiance_resolution,
        std::uint32_t radiance_resolution,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void upload_uniforms();
};
}
#endif