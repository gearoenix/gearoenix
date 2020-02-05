#include "rnd-sky-manager.hpp"

gearoenix::render::skybox::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::render::skybox::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::skybox::Skybox> gearoenix::render::skybox::Manager::get_gx3d(
    const core::Id mid,
    core::sync::EndCaller<Skybox>& c) noexcept
{
    GXUNIMPLEMENTED
    return std::shared_ptr<Skybox>();
}
