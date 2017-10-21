#ifndef GEAROEMIX_RENDER_SHADER_STAGE_VERTEX_HPP
#define GEAROEMIX_RENDER_SHADER_STAGE_VERTEX_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../../vulkan/shader/stage/vk-shader-stage-vertex.hpp"
#elif defined(USE_OPENGL)
#include "../../../opengl/shader/stage/gl-shd-stg-vertex.hpp"
#else
#error "Not implemented"
#endif // USE_*
#endif // GEAROEMIX_RENDER_SHADER_STAGE_VERTEX_HPP
