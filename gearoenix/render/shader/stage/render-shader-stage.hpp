#ifndef GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
#define GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../../vulkan/shader/stage/vk-shader-stage.hpp"
#else
#error "Not implemented"
#endif // USE_*
#endif // GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
