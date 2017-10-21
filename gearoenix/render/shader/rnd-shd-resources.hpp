#ifndef GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
#define GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/shader/vk-shd-resources.hpp"
#elif defined(USE_OPENGL_ES2)
#include "../../gles2/shader/gles2-shd-resources.hpp"
#else
#error "Unimplemented"
#endif
#endif
