#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/reflection/gx-rnd-rfl-manager.hpp"

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::reflection {
struct Manager final : public render::reflection::Manager {
private:
    engine::Engine& vk_e;

    std::shared_ptr<render::reflection::Builder> build_baked(
        const std::string& name,
        const std::shared_ptr<render::texture::TextureCube>& irradiance,
        const std::shared_ptr<render::texture::TextureCube>& radiance,
        const math::Aabb3<double>& include_box,
        const core::job::EndCaller& end_callback) override;
    std::shared_ptr<render::reflection::Builder> build_runtime(
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::uint64_t_t environment_resolution,
        std::uint64_t irradiance_resolution,
        std::uint64_t radiance_resolution,
        const core::job::EndCaller& end_callback) override;
    void update() override;

public:
    explicit Manager(engine::Engine& e);
    ~Manager() override;
};
}
#endif