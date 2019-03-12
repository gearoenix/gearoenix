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

gearoenix::render::buffer::Uniform * gearoenix::gles2::buffer::Manager::create_uniform(const unsigned int size)
{
	return new Uniform(size, std::static_pointer_cast<engine::Engine>(e));
}

#endif