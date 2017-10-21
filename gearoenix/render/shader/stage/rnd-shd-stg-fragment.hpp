#ifndef GEAROEMIX_RENDER_SHADER_STAGE_FRAGMENT_HPP
#define GEAROEMIX_RENDER_SHADER_STAGE_FRAGMENT_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../../vulkan/shader/stage/vk-shader-stage-fragment.hpp"
#elif defined(USE_OPENGL_ES2)
#include "../../../gles2/shader/stage/gles2-shd-stg-fragment.hpp"
#else
#error "Not implemented"
#endif // USE_*
#endif // GEAROEMIX_RENDER_SHADER_STAGE_FRAGMENT_HPP
