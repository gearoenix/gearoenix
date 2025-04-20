#ifndef GEAROENIX_VULKAN_MODEL_BUILDER_HPP
#define GEAROENIX_VULKAN_MODEL_BUILDER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/model/gx-rnd-mdl-builder.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::model {
struct Builder final : public render::model::Builder {
    friend struct Manager;

private:
    engine::Engine& vk_e;

    Builder(
        engine::Engine& e,
        const std::string& name,
        std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
        bool is_transformable);

    void set_material(const render::material::Pbr& material_type) override;

public:
    ~Builder() override;
};
}

#endif
#endif