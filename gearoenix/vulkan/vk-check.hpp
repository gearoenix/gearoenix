#pragma once
#include "../system/sys-log.hpp"
#include "vk-linker.hpp"
#include <string>

#define VKC(func)                                                                         \
    {                                                                                     \
        VkResult res = func;                                                              \
        if (VK_SUCCESS != res) {                                                          \
            LOGF(std::string("Error in vulkan, result is ") + std::to_string(res) + "."); \
        }                                                                                 \
    }
