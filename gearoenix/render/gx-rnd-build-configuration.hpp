#pragma once
#include "../platform/gx-plt-build-configuration.hpp"

#if GX_PLATFORM_WINDOWS
#define GX_RENDER_VULKAN_ENABLED true
#if !GX_PLATFORM_INTERFACE_SDL
#define GX_RENDER_DIRECT3D_ENABLED true
#else
#define GX_RENDER_OPENGL_ENABLED true
#endif
#elif GX_PLATFORM_LINUX
// #define GX_RENDER_OPENGL_ENABLED true
#define GX_RENDER_VULKAN_ENABLED true
#elif GX_PLATFORM_APPLE
// #define GX_RENDER_METAL_ENABLED true
#define GX_RENDER_VULKAN_ENABLED true
#define GX_RENDER_OPENGL_ENABLED true
#elif GX_PLATFORM_ANDROID
#define GX_RENDER_OPENGL_ENABLED true
// #define GX_RENDER_VULKAN_ENABLED true
#elif GX_PLATFORM_WEBASSEMBLY
#define GX_RENDER_OPENGL_ENABLED true
#else
#error "Unexpected/unimplemented platform."
#endif