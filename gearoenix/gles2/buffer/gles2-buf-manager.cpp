#include "gles2-buf-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../system/sys-log.hpp"
#include "../engine/gles2-eng-engine.hpp"
#include "gles2-buf-uniform.hpp"
#include <cstring>

gearoenix::gles2::buffer::Manager::Manager(const std::shared_ptr<engine::Engine>& e)
    : render::buffer::Manager(e)
{
}

gearoenix::gles2::buffer::Manager::~Manager()
{
}

std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::gles2::buffer::Manager::create_uniform(const unsigned int size, const core::sync::EndCaller<core::sync::EndCallerIgnore>& )
{
	return std::shared_ptr<render::buffer::Uniform>(new Uniform(size, std::static_pointer_cast<engine::Engine>(e)));
}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::gles2::buffer::Manager::create_static(const std::vector<math::BasicVertex> vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c)
{

}

std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::gles2::buffer::Manager::create_static(const std::vector<std::uint32_t> indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c)
{

}

#endif