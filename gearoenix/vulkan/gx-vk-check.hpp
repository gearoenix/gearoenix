#pragma once
#include "../core/gx-cr-build-configuration.hpp"
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-log.hpp"
#include "gx-vk-loader.hpp"
#include <string>

namespace gearoenix::vulkan {
const char* result_to_string(VkResult res);
}

#define GX_VK_CHK(func)                                                  \
    {                                                                    \
        const VkResult GX_CONCAT_2(_vk_res_, __LINE__) = func;           \
        if (VK_SUCCESS != GX_CONCAT_2(_vk_res_, __LINE__)) {             \
            GX_LOG_F(result_to_string(GX_CONCAT_2(_vk_res_, __LINE__))); \
        }                                                                \
    }                                                                    \
    static_assert(true, "")

#endif