#include "gx-vk-mat-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "gx-vk-mat-pbr.hpp"
#include "gx-vk-mat-sprite.hpp"
#include "gx-vk-mat-unlit.hpp"

gearoenix::vulkan::material::Manager::Manager()
    : Singleton<Manager>(this)
    , uniform_indexer(Pbr::max_count + Unlit::max_count + Sprite::max_count)
{
    core::Object::register_type<Material, Pbr>();

    core::Object::register_type<Pbr>();
    core::Object::register_type<Sprite>();
    core::Object::register_type<Unlit>();
}

gearoenix::vulkan::material::Manager::~Manager() = default;

void gearoenix::vulkan::material::Manager::construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    Pbr::construct(std::move(name), std::move(c));
}

void gearoenix::vulkan::material::Manager::construct_unlit(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c)
{
    Unlit::construct(std::move(name), std::move(c));
}

void gearoenix::vulkan::material::Manager::construct_sprite(std::string&&, core::job::EndCallerShared<render::material::Sprite>&&)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::material::Manager::upload_uniforms()
{
    uniform_indexer.update();
}

#endif