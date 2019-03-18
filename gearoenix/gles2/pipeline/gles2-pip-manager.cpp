#include "gles2-pip-manager.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../engine/gles2-eng-engine.hpp"
#include "../../render/pipeline/rnd-pip-resource.hpp"
#include "../buffer/gles2-buf-uniform.hpp"

gearoenix::gles2::pipeline::Manager::Manager(const std::shared_ptr<engine::Engine>& engine)
    : render::pipeline::Manager(engine)
{
}

gearoenix::gles2::pipeline::Manager::~Manager() {}

std::shared_ptr<gearoenix::render::pipeline::Pipeline> gearoenix::gles2::pipeline::Manager::get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline> &end)
{
    GXUNIMPLEMENTED;
}

std::shared_ptr<gearoenix::render::pipeline::Resource> gearoenix::gles2::pipeline::Manager::create_resource(const std::shared_ptr<render::buffer::Uniform> &u, const std::vector<std::shared_ptr<render::texture::Texture>>& ts)
{
    const std::shared_ptr<render::buffer::Buffer> b = std::static_pointer_cast<render::buffer::Buffer>(u);
    const render::pipeline::ResourceIdentifier rid(b, ts);
    const std::function<std::shared_ptr<render::pipeline::Resource>()> fun = [b, ts]
    {
        return std::make_shared<render::pipeline::Resource>(b, ts);
    };
    return resources.get(rid, fun);
}

#endif
