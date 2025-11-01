#pragma once
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"

#if GX_DEBUG_MODE
#define GX_GL_LABELING_ENABLED true
#endif

namespace gearoenix::gl {
#if GX_GL_LABELING_ENABLED
inline void set_buffer_label(const uint value, const std::string& name)
{
    if (0 == reinterpret_cast<std::uintptr_t>(glObjectLabel)) {
        return;
    }
    glObjectLabel(GL_BUFFER, value, static_cast<sizei>(name.size()), name.data());
}

inline void set_vertex_array_label(const uint value, const std::string& name)
{
    if (0 == reinterpret_cast<std::uintptr_t>(glObjectLabel)) {
        return;
    }
    glObjectLabel(GL_VERTEX_ARRAY, value, static_cast<sizei>(name.size()), name.data());
}

inline void set_texture_label(const uint value, const std::string& name)
{
    if (0 == reinterpret_cast<std::uintptr_t>(glObjectLabel)) {
        return;
    }
    glObjectLabel(GL_TEXTURE, value, static_cast<sizei>(name.size()), name.data());
}

inline void set_framebuffer_label(const uint value, const std::string& name)
{
    if (0 == reinterpret_cast<std::uintptr_t>(glObjectLabel)) {
        return;
    }
    glObjectLabel(GL_FRAMEBUFFER, value, static_cast<sizei>(name.size()), name.data());
}

inline void push_debug_group(const std::string& name)
{
    if (0 == reinterpret_cast<std::uintptr_t>(glPushDebugGroup)) {
        return;
    }
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<sizei>(name.size()), name.data());
}

inline void pop_debug_group()
{
    if (0 == reinterpret_cast<std::uintptr_t>(glPopDebugGroup)) {
        return;
    }
    glPopDebugGroup();
}
#else
#define set_buffer_label(value, name) static_assert(true, "")
#define set_vertex_array_label(value, name) static_assert(true, "")
#define set_texture_label(value, name) static_assert(true, "")
#define push_debug_group(name) static_assert(true, "")
#define pop_debug_group() static_assert(true, "")
#endif
}
#endif