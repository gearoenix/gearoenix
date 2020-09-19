#ifndef GEAROENIX_VULKAN_CHECK_HPP
#define GEAROENIX_VULKAN_CHECK_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../system/gx-sys-log.hpp"
#include "gx-vk-loader.hpp"
#include <string>

namespace gearoenix::vulkan {
const char* result_to_string(VkResult res) noexcept;
}

#define GX_VK_CHK_L(func)                                              \
    {                                                                  \
        const VkResult GX_CONCAT_2(_vk_res_, __LINE__) = Loader::func; \
        if (VK_SUCCESS != GX_CONCAT_2(_vk_res_, __LINE__)) {           \
            GXLOGF(result_to_string(GX_CONCAT_2(_vk_res_, __LINE__)))  \
        }                                                              \
    }

#define GX_VK_CHK(func)                                               \
    {                                                                 \
        const VkResult GX_CONCAT_2(_vk_res_, __LINE__) = func;        \
        if (VK_SUCCESS != GX_CONCAT_2(_vk_res_, __LINE__)) {          \
            GXLOGF(result_to_string(GX_CONCAT_2(_vk_res_, __LINE__))) \
        }                                                             \
    }

#endif
#endif
