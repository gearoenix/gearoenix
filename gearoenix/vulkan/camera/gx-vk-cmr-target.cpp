#include "gx-vk-cmr-target.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../texture/gx-vk-txt-target.hpp"


gearoenix::vulkan::camera::Target::Target(const render::camera::Target& rt)
    : target([&] {
        if (rt.is_default()) {
            const auto& [main, targets] = rt.get_default();
            Default d{};
            d.main = std::dynamic_pointer_cast<Target>(main);
            for (std::uint32_t ti = 0; ti < targets.size(); ++ti) {
                for (std::uint32_t mi = 0; mi < targets[ti].size(); ++mi) {
                    d.targets[ti][mi] = std::dynamic_pointer_cast<Target>(targets[ti][mi]);
                }
            }
            return target_t{ std::move(d) };
        }
        if (rt.is_customised()) {
            return target_t { Customised { .target = std::dynamic_pointer_cast<Target>(rt.get_customised().target) } };
        }
        GX_UNEXPECTED;
    }())
{
}

gearoenix::vulkan::camera::Target::~Target() = default;

const gearoenix::vulkan::camera::Target::Customised& gearoenix::vulkan::camera::Target::get_customised() const
{
    GX_ASSERT_D(customised_var_index == target.index());
    return std::get<customised_var_index>(target);
}

const gearoenix::vulkan::camera::Target::Default& gearoenix::vulkan::camera::Target::get_default() const
{
    GX_ASSERT_D(default_var_index == target.index());
    return std::get<default_var_index>(target);
}


#endif