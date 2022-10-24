#ifndef GEAROENIX_VULKAN_REFLECTION_MANAGER_HPP
#define GEAROENIX_VULKAN_REFLECTION_MANAGER_HPP
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
        const core::sync::EndCallerIgnored& end_callback) noexcept final;
    std::shared_ptr<render::reflection::Builder> build_runtime(
        const std::string& name,
        const math::Aabb3<double>& receive_box,
        const math::Aabb3<double>& exclude_box,
        const math::Aabb3<double>& include_box,
        std::size_t environment_resolution,
        std::size_t irradiance_resolution,
        std::size_t radiance_resolution,
        const core::sync::EndCallerIgnored& end_callback) noexcept final;
    void update() noexcept final;

public:
    explicit Manager(engine::Engine& e) noexcept;
    ~Manager() noexcept final;
};
}

#endif
#endif