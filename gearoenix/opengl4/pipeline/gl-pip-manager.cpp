#include "gl-pip-manager.hpp"
#ifdef USE_OPENGL
#include "../../system/sys-log.hpp"
gearoenix::render::pipeline::Manager::Manager(Engine*)
{
    TODO;
}

gearoenix::render::pipeline::Manager::~Manager()
{
    TODO;
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_pipeline(shader::Id)
{
    TODO;
    return nullptr;
}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::render::pipeline::Manager::get_cached_pipeline(shader::Id)
{
    TODO;
    return nullptr;
}

gearoenix::render::descriptor::Pool* gearoenix::render::pipeline::Manager::get_descriptor_pool()
{
    TODO;
    return nullptr;
}

#endif
