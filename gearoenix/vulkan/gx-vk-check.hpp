#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/macro/gx-cr-mcr-concatenate.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-vk-loader.hpp"

#define GX_VK_CHK_L(func)                                                           \
    {                                                                               \
        const auto GX_CONCAT_2(_vk_res_, __LINE__) = static_cast<vk::Result>(func); \
        if (vk::Result::eSuccess != GX_CONCAT_2(_vk_res_, __LINE__)) {              \
            GX_LOG_F(vk::to_string(GX_CONCAT_2(_vk_res_, __LINE__)));               \
        }                                                                           \
    }                                                                               \
    static_assert(true, "")

#endif
