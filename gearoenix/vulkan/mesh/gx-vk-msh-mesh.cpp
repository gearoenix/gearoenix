#include "gx-vk-msh-mesh.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/gx-cr-object.hpp"
#include "../../render/record/gx-rnd-rcd-camera.hpp"
#include "../../render/record/gx-rnd-rcd-model.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../descriptor/gx-vk-des-bindless.hpp"
#include "../pipeline/gx-vk-pip-push-constant.hpp"
#include "../material/gx-vk-mat-material.hpp"
#include "../../render/material/gx-rnd-mat-material.hpp"
#include "gx-vk-msh-buffer.hpp"

gearoenix::vulkan::mesh::Mesh::Mesh(
    std::string&& name,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material)
    : render::mesh::Mesh(std::move(name), std::move(buffer), std::move(material))
    , gapi_buffer(core::cast_shared<Buffer>(std::shared_ptr(this->buffer)))
    , gapi_material(std::dynamic_pointer_cast<material::Material>(this->bound_material))
{
}

void gearoenix::vulkan::mesh::Mesh::construct(
    std::string&& name,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    const core::job::EndCallerShared<render::mesh::Mesh>& end_callback)
{
    end_callback.set_return(Object::construct<Mesh>(std::move(name), std::move(buffer), std::move(material)));
}

gearoenix::vulkan::mesh::Mesh::~Mesh() = default;

void gearoenix::vulkan::mesh::Mesh::draw(const VkCommandBuffer cmd, pipeline::PushConstants& pc) const
{
    const auto& vertex_buffer = *gapi_buffer->get_vertex();
    const auto vk_vertex_buffer = vertex_buffer.get_vulkan_data();

    const auto vertex_offset = static_cast<VkDeviceSize>(vertex_buffer.get_allocated_memory()->get_allocator()->get_offset());
    vkCmdBindVertexBuffers(cmd, 0, 1, &vk_vertex_buffer, &vertex_offset);

    const auto& index_buffer = *gapi_buffer->get_index();
    const auto index_offset = static_cast<VkDeviceSize>(index_buffer.get_allocated_memory()->get_allocator()->get_offset());
    vkCmdBindIndexBuffer(cmd, index_buffer.get_vulkan_data(), index_offset, VK_INDEX_TYPE_UINT32);

    vkCmdPushConstants(cmd, descriptor::Bindless::get().get_pipeline_layout(), VK_SHADER_STAGE_ALL, 0, sizeof(pc), &pc);

    vkCmdDrawIndexed(cmd, gapi_buffer->get_indices_count(), 1, 0, 0, 0);
}

void gearoenix::vulkan::mesh::Mesh::set_material(std::shared_ptr<render::material::Material>&& material)
{
    render::mesh::Mesh::set_material(std::move(material));
    gapi_material = std::dynamic_pointer_cast<material::Material>(this->bound_material);
}

#endif