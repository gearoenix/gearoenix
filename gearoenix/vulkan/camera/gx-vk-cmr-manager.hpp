#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-manager.hpp"
#include "../descriptor/gx-vk-des-uniform-holder.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

namespace gearoenix::vulkan::camera {
struct Manager final : render::camera::Manager, core::Singleton<Manager> {
private:
    descriptor::UniformIndexer<GxShaderDataCamera> camera_uniform_indexer;
    descriptor::UniformIndexer<GxShaderDataCameraJointModel> cameras_joint_models_uniform_indexer;

public:
    Manager();
    ~Manager() override;
    void build(
        std::string&& name,
        core::ecs::Entity* parent,
        core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback) override;
    void window_resized() override;
    void update() override;
};
}

#endif