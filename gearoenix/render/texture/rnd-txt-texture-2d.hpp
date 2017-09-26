#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/texture/vk-txt-texture-2d.hpp"
#else
#error "Unimplemented"
#endif
#endif
