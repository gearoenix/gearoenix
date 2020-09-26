#include "gx-glc3-buf-manager.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../engine/gx-glc3-eng-engine.hpp"
#include "gx-glc3-buf-index.hpp"
#include "gx-glc3-buf-uniform.hpp"
#include "gx-glc3-buf-vertex.hpp"
#include <cstring>

gearoenix::glc3::buffer::Manager::Manager(engine::Engine* const e) noexcept
    : render::buffer::Manager(e)
{
}

std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::glc3::buffer::Manager::create_uniform(const std::size_t size) noexcept
{
    return std::make_shared<Uniform>(size, dynamic_cast<engine::Engine*>(e));
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::glc3::buffer::Manager::create_static(const std::vector<math::BasicVertex>& vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return std::make_shared<Vertex>(vertices, dynamic_cast<engine::Engine*>(e), c);
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::glc3::buffer::Manager::create_static(const std::vector<std::uint32_t>& indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    return std::make_shared<Index>(indices, dynamic_cast<engine::Engine*>(e), c);
}

#endif