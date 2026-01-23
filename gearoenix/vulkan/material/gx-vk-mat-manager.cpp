#include "gx-vk-mat-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"

#include <atomic>

namespace {
std::vector<GxShaderDataMaterial> shader_datas;
std::atomic<std::uint32_t> shader_data_last_index = 0;
std::shared_ptr<gearoenix::vulkan::buffer::Uniform> uniform_buffer;
}

gearoenix::vulkan::material::Manager::Manager() : Singleton<Manager>(this)
{
}

gearoenix::vulkan::material::Manager::~Manager() = default;



const gearoenix::vulkan::buffer::Uniform& gearoenix::vulkan::material::Manager::get_uniform_buffer()
{
    return *uniform_buffer;
}

void gearoenix::vulkan::material::Manager::construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c)
{
    GX_UNIMPLEMENTED;
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