#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

namespace gearoenix::vulkan::model {
struct Manager final : render::model::Manager, core::Singleton<Manager> {
private:
    descriptor::UniformIndexer<GxShaderDataModel> model_uniform_indexer;
    descriptor::UniformIndexer<GxShaderDataBone> bone_uniform_indexer;

public:
    Manager();
    ~Manager() override;
};
}
#endif