#ifndef GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
#define GEAROEMIX_RENDER_SHADER_RESOURCES_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/shader/vk-shd-resources.hpp"
#elif defined(USE_OPENGL)
#include "../../opengl/shader/gl-shd-resources.hpp"
#else
#error "Unimplemented"
#endif
#endif
