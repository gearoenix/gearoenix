#ifndef GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
#define GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "gx-vk-pip-cache.hpp"

namespace gearoenix::vulkan::pipeline {
struct Manager final {
    GX_GET_CREF_PRV(Cache, cache)

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
