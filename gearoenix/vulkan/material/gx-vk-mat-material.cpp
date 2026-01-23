#include "gx-vk-mat-material.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../platform/gx-plt-log.hpp"

gearoenix::vulkan::material::Material::~Material() = default;

void gearoenix::vulkan::material::Material::bind_forward(pipeline::Pipeline*& pipeline)
{
    GX_UNIMPLEMENTED;
}

void gearoenix::vulkan::material::Material::bind_shadow(pipeline::Pipeline*& pipeline)
{
    GX_UNIMPLEMENTED;
}

#endif