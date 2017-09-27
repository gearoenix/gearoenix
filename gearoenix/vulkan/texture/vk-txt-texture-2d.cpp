#include "vk-txt-texture-2d.hpp"
#include "../../core/cr-static.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/vk-buf-manager.hpp"
#include "../buffer/vk-buf-sub-buffer.hpp"
#include "../command/vk-cmd-buffer.hpp"
#include "../device/vk-dev-logical.hpp"
#include "../image/vk-img-image.hpp"
#include "../image/vk-img-view.hpp"
#include "../memory/vk-mem-memory.hpp"
#include "../vk-check.hpp"
#include "../vk-engine.hpp"

gearoenix::render::texture::Texture2D::Texture2D(system::File* file, Engine* engine)
{
    const Linker* l = engine->get_linker();
    std::vector<unsigned char> pixels;
    unsigned int img_width, img_height, channels;
    PNG::decode(file, engine, pixels, img_width, img_height, channels);
    VkImageCreateInfo image_info;
    device::Logical* dev = engine->get_logical_device();
    memory::Manager* vmemmgr = engine->get_v_memory_manager();
    fill_info(image_info, img_width, img_height, channels);
    image::Image* img = new image::Image(dev, image_info, vmemmgr);
    buffer::Manager* stbuf = engine->get_cpu_buffer_manager();
    buffer::SubBuffer* sstbuf = stbuf->create_subbuffer(pixels.size());
    sstbuf->write(pixels.data(), pixels.size());
    std::function<std::function<void()>(command::Buffer*)> todo = [this, img, l, sstbuf](command::Buffer* c) {
        img->transit_for_writing(c);
        img->copy_from_buffer(c, sstbuf);
        img->transit_for_reading(c);
        std::function<void()> fn = [sstbuf] {
            delete sstbuf;
        };
        return fn;
    };
    engine->push_todo(todo);
    iv = new image::View(img, img->get_format());
    smp = engine->get_sampler_2d();
}

gearoenix::render::texture::Texture2D::~Texture2D()
{
    delete iv;
}

uint32_t gearoenix::render::texture::Texture2D::get_memory_type_bits(Engine* engine)
{
    VkImageCreateInfo image_info[2];
    VkMemoryRequirements memreqs[2];
    const Linker* l = engine->get_linker();
    const VkDevice vkdev = engine->get_logical_device()->get_vulkan_data();
    VkImage vkimg;
    fill_info(image_info[0], 1024, 1024, 4);
    VKC(l->vkCreateImage(vkdev, &image_info[0], nullptr, &vkimg));
    l->vkGetImageMemoryRequirements(vkdev, vkimg, &memreqs[0]);
    l->vkDestroyImage(vkdev, vkimg, nullptr);
    fill_info(image_info[1], 1024, 1024, 3);
    VKC(l->vkCreateImage(vkdev, &image_info[1], nullptr, &vkimg));
    l->vkGetImageMemoryRequirements(vkdev, vkimg, &memreqs[1]);
    l->vkDestroyImage(vkdev, vkimg, nullptr);
    if (memreqs[0].memoryTypeBits == memreqs[1].memoryTypeBits)
        LOGE("Unnecessary!");
    return memreqs[0].memoryTypeBits | memreqs[1].memoryTypeBits;
}

void gearoenix::render::texture::Texture2D::fill_info(
    VkImageCreateInfo& image_info,
    unsigned int img_width,
    unsigned int img_height,
    unsigned int channels)
{
    setz(image_info);
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = static_cast<uint32_t>(img_width);
    image_info.extent.height = static_cast<uint32_t>(img_height);
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    switch (channels) {
    case 4:
        image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        break;
    case 3:
        image_info.format = VK_FORMAT_R8G8B8_UNORM;
        break;
    default:
        LOGF("Unexpected/Unimplemented");
    }
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
}
