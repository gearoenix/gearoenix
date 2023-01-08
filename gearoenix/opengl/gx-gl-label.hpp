#ifndef GEAROENIX_GL_LABEL_HPP
#define GEAROENIX_GL_LABEL_HPP
#include "../core/gx-cr-build-configuration.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-loader.hpp"

#ifdef GX_DEBUG_MODE
#define GX_GL_LABELING_ENABLED
#endif

namespace gearoenix::gl {
#ifdef GX_GL_LABELING_ENABLED
inline void set_buffer_label(uint value, const std::string& name) noexcept
{
    glObjectLabel(GL_BUFFER, value, static_cast<sizei>(name.size()), name.data());
}

inline void set_vertex_array_label(uint value, const std::string& name) noexcept
{
    glObjectLabel(GL_VERTEX_ARRAY, value, static_cast<sizei>(name.size()), name.data());
}

inline void push_debug_group(const std::string& name) noexcept
{
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<sizei>(name.size()), name.data());
}

inline void pop_debug_group() noexcept
{
    glPopDebugGroup();
}
#else
#define set_buffer_label(value, name) static_assert(true, "")
#define set_vertex_array_label(value, name) static_assert(true, "")
#define push_debug_group(name) static_assert(true, "")
#define pop_debug_group() static_assert(true, "")
#endif
}
#endif