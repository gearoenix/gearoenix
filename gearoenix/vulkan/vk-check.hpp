#pragma once
#include <string>
#include "vk-linker.hpp"
#include "../system/sys-log.hpp"

#define VKC(func) { \
    VkResult res = func; \
    if (VK_SUCCESS != res) { \
        LOGF(std::string("Error in vulkan, result is ") + std::to_string(res) + "."); \
    } \
}
