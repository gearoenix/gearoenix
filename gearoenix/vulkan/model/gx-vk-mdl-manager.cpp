#include "gx-vk-mdl-manager.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-mdl-builder.hpp"

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::vulkan::model::Manager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    const core::sync::EndCaller& c,
    const bool is_transformable) noexcept
{
    return std::shared_ptr<Builder>(new Builder(vk_e, name, std::move(mesh), is_transformable));
}

gearoenix::vulkan::model::Manager::Manager(engine::Engine& e) noexcept
    : render::model::Manager(e)
    , vk_e(e)
{
}

gearoenix::vulkan::model::Manager::~Manager() noexcept = default;

#endif
