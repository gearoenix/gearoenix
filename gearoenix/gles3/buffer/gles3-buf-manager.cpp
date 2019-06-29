#include "gles3-buf-manager.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../system/sys-log.hpp"
#include "../engine/gles3-eng-engine.hpp"
#include "gles3-buf-index.hpp"
#include "gles3-buf-uniform.hpp"
#include "gles3-buf-vertex.hpp"
#include <cstring>

gearoenix::gles3::buffer::Manager::Manager(engine::Engine* const e) noexcept
    : render::buffer::Manager(e)
{
}

gearoenix::render::buffer::Uniform* gearoenix::gles3::buffer::Manager::create_uniform(const unsigned int size) noexcept
{
    return new Uniform(size, dynamic_cast<engine::Engine*>(e));
}

gearoenix::render::buffer::Static* gearoenix::gles3::buffer::Manager::create_static(std::vector<math::BasicVertex> vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return new Vertex(std::move(vertices), dynamic_cast<engine::Engine*>(e), c);
}

gearoenix::render::buffer::Static* gearoenix::gles3::buffer::Manager::create_static(std::vector<std::uint32_t> indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return new Index(std::move(indices), dynamic_cast<engine::Engine*>(e), c);
}

#endif