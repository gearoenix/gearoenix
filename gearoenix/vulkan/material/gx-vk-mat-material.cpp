#include "gx-vk-mat-material.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"
#include "../descriptor/gx-vk-des-uniform-indexer.hpp"

gearoenix::vulkan::material::Material::Material()
    : shader_data(uniform_indexer_t::get().get_next())
{
}

gearoenix::vulkan::material::Material::~Material() = default;

void gearoenix::vulkan::material::Material::set(const bool, DrawCache&)
{
    GX_UNIMPLEMENTED; // needs to be implemented by the child type
}

#endif
