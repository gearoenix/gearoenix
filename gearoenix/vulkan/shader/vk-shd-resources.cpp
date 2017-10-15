#include "vk-shd-resources.hpp"
#include "../../core/cr-static.hpp"
#include "../buffer/vk-buf-buffer.hpp"
#include "../buffer/vk-buf-sub-buffer.hpp"
#include "../buffer/vk-buf-uniform.hpp"
#include "../descriptor/vk-des-set.hpp"
#include "../image/vk-img-view.hpp"
#include "../pipeline/vk-pip-manager.hpp"
#include "../pipeline/vk-pip-pipeline.hpp"
#include "../texture/vk-txt-sampler-2d.hpp"
#include "../texture/vk-txt-texture-2d.hpp"
#include "../vk-engine.hpp"

gearoenix::render::shader::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
{
    dc = u->get_count();
    dessets = new descriptor::Set*[dc];
    for (unsigned int i = 0; i < dc; ++i) {
        VkDescriptorBufferInfo inf;
        setz(inf);
        inf.buffer = u->get_vbuf()[i]->get_buffer()->get_vulkan_data();
        inf.offset = u->get_vbuf()[i]->get_offset();
        inf.range = u->get_size();
        dessets[i] = new descriptor::Set(e->get_pipeline_manager()->get_descriptor_pool(), pip->get_descriptor_set_layout(), inf);
    }
}

gearoenix::render::shader::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u, texture::Texture2D* t)
{
    dc = u->get_count();
    dessets = new descriptor::Set*[dc];
    for (unsigned int i = 0; i < dc; ++i) {
        VkDescriptorBufferInfo inf;
        setz(inf);
        inf.buffer = u->get_vbuf()[i]->get_buffer()->get_vulkan_data();
        inf.offset = u->get_vbuf()[i]->get_offset();
        inf.range = u->get_size();
        VkDescriptorImageInfo imginf;
        setz(inf);
        imginf.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imginf.imageView = t->get_view()->get_vulkan_data();
        imginf.sampler = e->get_sampler_2d()->get_vulkan_data();
        dessets[i] = new descriptor::Set(e->get_pipeline_manager()->get_descriptor_pool(), pip->get_descriptor_set_layout(), inf, imginf);
    }
}

gearoenix::render::shader::Resources::~Resources()
{
    for (unsigned int i = 0; i < dc; ++i) {
        delete dessets[i];
    }
    delete[] dessets;
}
