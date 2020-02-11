#include "rnd-cmd-buffer.hpp"
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"

void gearoenix::render::command::Buffer::begin() noexcept
{
    recorded_secondaries.clear();
    bound_resource_sets.clear();
    render_target = nullptr;
}

void gearoenix::render::command::Buffer::end() noexcept
{
}

void gearoenix::render::command::Buffer::record(Buffer* const o) noexcept
{
    recorded_secondaries.push_back(o);
}

void gearoenix::render::command::Buffer::bind(pipeline::ResourceSet* const r) noexcept
{
    bound_resource_sets.push_back(r);
}

void gearoenix::render::command::Buffer::bind(const texture::Target* const t) noexcept
{
#ifdef GX_DEBUG_MODE
    if (render_target != nullptr)
        GXLOGF("Only one render target must be bound.")
#endif
    render_target = t;
}