#include "gl-pip-pipeline.hpp"
#ifdef USE_OPENGL
#include "../../system/sys-log.hpp"
gearoenix::render::pipeline::Pipeline::Pipeline(
    shader::Id,
    Cache*,
    RenderPass*,
    const std::shared_ptr<shader::Shader>&,
    descriptor::SetLayout*,
    Engine*)
{
    TODO;
}

gearoenix::render::pipeline::Pipeline::~Pipeline() { TODO; }

gearoenix::render::descriptor::SetLayout* gearoenix::render::pipeline::Pipeline::get_descriptor_set_layout()
{
    TODO;
    return nullptr;
}

const gearoenix::render::pipeline::Layout* gearoenix::render::pipeline::Pipeline::get_layout() const
{
    TODO;
    return nullptr;
}

void gearoenix::render::pipeline::Pipeline::bind() { TODO; }

#endif
