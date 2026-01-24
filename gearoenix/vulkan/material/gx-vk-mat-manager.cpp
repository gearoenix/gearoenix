#include "gx-vk-mat-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-mat-pbr.hpp"
#include "gx-vk-mat-unlit.hpp"
#include "gx-vk-mat-sprite.hpp"

gearoenix::vulkan::material::Manager::Manager()
    : Singleton<Manager>(this)
    , UniformHolder(Pbr::max_count)
{
}

gearoenix::vulkan::material::Manager::~Manager() = default;

void gearoenix::vulkan::material::Manager::construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    Pbr::construct(std::move(name), std::move(c));
}

void gearoenix::vulkan::material::Manager::construct_unlit(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::material::Manager::construct_sprite(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c)
{
    GX_UNIMPLEMENTED;
}

#endif