#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/camera/gx-rnd-cmr-target.hpp"

#include <array>
#include <memory>
#include <variant>

namespace gearoenix::vulkan::texture {
struct Target;
}

namespace gearoenix::vulkan::camera {
struct Target final {
    struct Customised final {
        std::shared_ptr<texture::Target> target;
        // TODO: we can cache some in here and in default later if it can improve performance
    };

    struct Default final {
        std::shared_ptr<texture::Target> main;
        std::array<std::array<std::shared_ptr<texture::Target>, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT>, 2> targets;
        // TODO: maybe I need to cache more things here for better performance
    };

    using target_t = std::variant<Customised, Default>;

    constexpr static std::uint32_t customised_var_index = 0;
    constexpr static std::uint32_t default_var_index = 1;

    target_t target = Default {};

    explicit Target(const render::camera::Target& rt);
    ~Target();
    [[nodiscard]] const Customised& get_customised() const;
    [[nodiscard]] const Default& get_default() const;
};
}

#endif