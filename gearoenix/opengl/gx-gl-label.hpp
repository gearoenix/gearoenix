#pragma once
#include "../core/gx-cr-build-configuration.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"

#if GX_DEBUG_MODE
#define GX_GL_LABELING_ENABLED !GX_PLATFORM_ANDROID
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

struct PushDebugGroup final {
    explicit PushDebugGroup(const std::string& name) noexcept
    {
        if (0 == reinterpret_cast<std::uintptr_t>(glPushDebugGroup)) {
            return;
        }
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<sizei>(name.size()), name.data());
    }

    ~PushDebugGroup() noexcept
    {
        if (0 == reinterpret_cast<std::uintptr_t>(glPopDebugGroup)) {
            return;
        }
        glPopDebugGroup();
    }
};
}

#include "../core/macro/gx-cr-mcr-concatenate.hpp"
#include <format>
#include <string>

#define GX_GL_PUSH_DEBUG_GROUP_STR_VAR GX_CONCAT(_gearoenix_opengl_push_debug_str_, __LINE__)
#define GX_GL_PUSH_DEBUG_GROUP_OBJ_VAR GX_CONCAT(_gearoenix_opengl_push_debug_obj_, __LINE__)

#define GX_GL_PUSH_DEBUG_GROUP(...)                                                  \
    thread_local std::string GX_GL_PUSH_DEBUG_GROUP_STR_VAR;                         \
    GX_GL_PUSH_DEBUG_GROUP_STR_VAR.clear();                                          \
    std::format_to(std::back_inserter(GX_GL_PUSH_DEBUG_GROUP_STR_VAR), __VA_ARGS__); \
    const gearoenix::gl::PushDebugGroup GX_GL_PUSH_DEBUG_GROUP_OBJ_VAR(GX_GL_PUSH_DEBUG_GROUP_STR_VAR);

#else
}
#define set_buffer_label(value, name) static_assert(true, "")
#define set_vertex_array_label(value, name) static_assert(true, "")
#define set_texture_label(value, name) static_assert(true, "")
#define set_framebuffer_label(value, name) static_assert(true, "")
#define GX_GL_PUSH_DEBUG_GROUP(...) static_assert(true, "")
#endif
#endif