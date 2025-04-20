#ifndef GEAROENIX_VULKAN_MODEL_MANAGER_HPP
#define GEAROENIX_VULKAN_MODEL_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-manager.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::model {
struct Manager final : public render::model::Manager {
    engine::Engine& vk_e;

private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        const core::job::EndCaller& c,
        bool is_transformable) override;

public:
    explicit Manager(engine::Engine& e);
    ~Manager() override;
};
}
#endif
#endif