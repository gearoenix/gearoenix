#include "rnd-cmd-buffer.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"

gearoenix::render::command::Buffer::~Buffer() {}

void gearoenix::render::command::Buffer::begin()
{
    recored_secondaries.clear();
    bound_resource_sets.clear();
    render_target = nullptr;
}

void gearoenix::render::command::Buffer::end()
{
}

void gearoenix::render::command::Buffer::record(const std::shared_ptr<Buffer>& o)
{
    recored_secondaries.push_back(o);
}

void gearoenix::render::command::Buffer::bind(const std::shared_ptr<pipeline::ResourceSet>& r)
{
    bound_resource_sets.push_back(r);
}

void gearoenix::render::command::Buffer::bind(texture::Target* const t)
{
#ifdef GX_DEBUG_MODE
    if (render_target != nullptr)
        GXLOGF("Only one render target must be bound.");
#endif
    render_target = t;
}