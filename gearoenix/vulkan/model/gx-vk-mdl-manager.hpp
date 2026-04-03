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
    [[nodiscard]] const descriptor::UniformIndexer<GxShaderDataModel>& get_model_uniform_indexer() const { return model_uniform_indexer; }
    [[nodiscard]] const descriptor::UniformIndexer<GxShaderDataBone>& get_bone_uniform_indexer() const { return bone_uniform_indexer; }

    Manager();
    ~Manager() override;

    [[nodiscard]] core::ecs::EntityPtr build(std::string&& name, core::ecs::Entity* parent, render::model::meshes_set_t&& meshes, bool is_transformable, bool is_skinned) override;
    void update() override;
    void upload_uniforms();
};
}
#endif