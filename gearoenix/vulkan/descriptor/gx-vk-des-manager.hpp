#ifndef GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "gx-vk-des-pool.hpp"

namespace gearoenix::vulkan::descriptor {
struct Manager final {
    GX_GET_REF_PRV(Pool, imgui)
public:
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    explicit Manager(const device::Logical& logical_device) noexcept;
    ~Manager() noexcept;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
};
}

#endif
#endif