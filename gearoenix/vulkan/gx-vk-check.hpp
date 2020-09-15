#ifndef GEAROENIX_VULKAN_CHECK_HPP
#define GEAROENIX_VULKAN_CHECK_HPP
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../system/gx-sys-log.hpp"
#include "gx-vk-loader.hpp"
#include <string>

#define GX_VK_CHK(func)                                         \
    {                                                           \
        VkResult res = func;                                    \
        if (VK_SUCCESS != res) {                                \
            GXLOGF("Error in vulkan, result is " << res << ".") \
        }                                                       \
    }

#endif
#endif
