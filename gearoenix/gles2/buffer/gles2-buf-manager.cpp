#include "gles2-buf-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-buf-index.hpp"
#include "gles2-buf-uniform.hpp"
#include "gles2-buf-vertex.hpp"
#include <cstring>

gearoenix::gles2::buffer::Manager::Manager(engine::Engine* const e) noexcept
    : render::buffer::Manager(e)
{
}

gearoenix::render::buffer::Uniform* gearoenix::gles2::buffer::Manager::create_uniform(const std::size_t size) noexcept
{
    return new Uniform(size, dynamic_cast<engine::Engine*>(e));
}

gearoenix::render::buffer::Static* gearoenix::gles2::buffer::Manager::create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return new Vertex(vertices, dynamic_cast<engine::Engine*>(e), c);
}

gearoenix::render::buffer::Static* gearoenix::gles2::buffer::Manager::create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return new Index(indices, dynamic_cast<engine::Engine*>(e), c);
}

#endif