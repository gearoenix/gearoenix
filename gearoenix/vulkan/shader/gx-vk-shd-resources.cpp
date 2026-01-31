#include "gx-vk-shd-resources.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../buffer/gx-vk-buf-buffer.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"
#include "../image/gx-vk-img-view.hpp"
#include "../pipeline/gx-vk-pip-manager.hpp"
#include "../pipeline/gx-vk-pip-pipeline.hpp"
#include "../engine/gx-vk-eng-engine.hpp"

gearoenix::vulkan::shader::Resources::Resources(pipeline::Pipeline* pip, buffer::Uniform* u)
{
    descriptors_count = 1;
    descriptors.resize(descriptors_count);
    for (unsigned int i = 0; i < descriptors_count; ++i) {
        VkDescriptorBufferInfo inf;
        GX_SET_ZERO(inf);
        inf.buffer = u->get_gpu()->get_vulkan_data();
        inf.offset = u->get_gpu()->get_allocator()->get_offset();
        inf.range = u->get_gpu()->get_allocator()->get_size();
        // descriptors[i] = new descriptor::Set(pipeline::Manager::get().get_descriptor_pool(), pip->get_descriptor_set_layout(), inf);
    }
}

gearoenix::vulkan::shader::Resources::Resources(pipeline::Pipeline* pip, buffer::Uniform* u, texture::Texture2D* t)
{
    descriptors_count = 1;
    descriptors.resize(descriptors_count);
    for (unsigned int i = 0; i < descriptors_count; ++i) {
        VkDescriptorBufferInfo inf;
        GX_SET_ZERO(inf);
        // inf.buffer = u->get_vbuf()[i]->get_buffer()->get_vulkan_data();
        // inf.offset = u->get_vbuf()[i]->get_offset();
        // inf.range = u->get_size();
        VkDescriptorImageInfo imginf;
        GX_SET_ZERO(imginf);
        // imginf.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        // imginf.imageView = t->get_view()->get_vulkan_data();
        // imginf.sampler = e->get_sampler_2d()->get_vulkan_data();
        // dessets[i] = new descriptor::Set(e->get_pipeline_manager()->get_descriptor_pool(), pip->get_descriptor_set_layout(), inf, imginf);
    }
}

gearoenix::vulkan::shader::Resources::~Resources() = default;

void gearoenix::vulkan::shader::Resources::bind(pipeline::Pipeline& pip)
{
    // descriptors[core::Singleton<engine::Engine>::get().get_frame_number()]->(p, cmd);
}
#endif
