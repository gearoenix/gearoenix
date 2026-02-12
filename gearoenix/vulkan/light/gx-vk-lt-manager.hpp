#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/light/gx-rnd-lt-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::light {
using directionals_uniform_indexer_t = descriptor::UniformIndexer<GxShaderDataDirectionalLight>;
using points_uniform_indexer_t = descriptor::UniformIndexer<GxShaderDataPointLight>;
using directional_shadow_casters_uniform_indexer_t = descriptor::UniformIndexer<GxShaderDataShadowCasterDirectionalLight>;

struct Manager final : render::light::Manager, core::Singleton<Manager> {
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataDirectionalLight>, directionals_uniform_indexer);
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataPointLight>, points_uniform_indexer);
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataShadowCasterDirectionalLight>, directional_shadow_casters_uniform_indexer);

public:
    Manager();
    ~Manager() override;
    void update() override;
    void upload_uniforms();
    [[nodiscard]] core::ecs::EntityPtr build_directional(std::string&&, core::ecs::Entity*) override;
    void build_shadow_caster_directional(
        std::string&& name,
        core::ecs::Entity* parent,
        std::uint32_t shadow_map_resolution,
        float camera_far,
        float camera_near,
        float camera_aspect,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
};
}
#endif