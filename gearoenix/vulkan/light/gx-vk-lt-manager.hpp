#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/light/gx-rnd-lt-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::light {
struct Manager final : render::light::Manager, core::Singleton<Manager> {
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataDirectionalLight>, directionals_uniform_indexer);
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataPointLight>, points_uniform_indexer);
    GX_GET_CREF_PRV(descriptor::UniformIndexer<GxShaderDataShadowCasterDirectionalLight>, directional_shadow_casters_uniform_indexer);

public:
    Manager();
    ~Manager() override;
    void update() override;
    void upload_uniforms();
};
}
#endif