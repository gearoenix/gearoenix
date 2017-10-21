#ifndef GEAROENIX_RENDER_BUFFER_MESH_HPP
#define GEAROENIX_RENDER_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/buffer/vk-buf-mesh.hpp"
#elif defined(USE_OPENGL_ES2)
#include "../../gles2/buffer/gles2-buf-mesh.hpp"
#else
#error "Not implemented yet"
#endif
#endif // GEAROENIX_RENDER_BUFFER_MESH_HPP
