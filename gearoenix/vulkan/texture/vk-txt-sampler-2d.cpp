#include "vk-txt-sampler-2d.hpp"
#include "../../core/cr-static.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../device/vk-dev-physical.hpp"
#include "../vk-check.hpp"
#include "../vk-instance.hpp"

gearoenix::render::texture::Sampler2D::Sampler2D(device::Logical* dev)
    : dev(dev)
{
    const VkDevice vkdev = dev->get_vulkan_data();
    const device::Physical* pdev = dev->get_physical_device();
    const Linker* l = pdev->get_instance()->get_linker();
    VkSamplerCreateInfo sampler_info;
    setz(sampler_info);
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;
    VKC(l->vkCreateSampler(vkdev, &sampler_info, nullptr, &vulkan_data));
}

gearoenix::render::texture::Sampler2D::~Sampler2D()
{
    const VkDevice vkdev = dev->get_vulkan_data();
    const device::Physical* pdev = dev->get_physical_device();
    const Linker* l = pdev->get_instance()->get_linker();
    l->vkDestroySampler(vkdev, vulkan_data, nullptr);
}

const VkSampler& gearoenix::render::texture::Sampler2D::get_vulkan_data() const
{
    return vulkan_data;
}
