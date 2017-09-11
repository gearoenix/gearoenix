#ifndef GEAROENIX_NUFRAG_VULKAN_IMAGE_VIEW_HPP
#define GEAROENIX_NUFRAG_VULKAN_IMAGE_VIEW_HPP
#include <memory>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace nufrag {
namespace render {
namespace memory {
class Pool;
}
namespace device {
class Logical;
}
namespace image {
class Image;
class View {
private:
	std::shared_ptr<Image> img;
	VkImageView vulkan_data;
public:
	View(const std::shared_ptr<Image> &img, const VkFormat &format);
    View(const std::shared_ptr<Image> &img, const VkImageViewCreateInfo &info);
	~View();
    static std::shared_ptr<View> create_depth_stencil(const std::shared_ptr<memory::Pool> &mem_pool);
	const std::shared_ptr<Image> &get_image() const;
	const VkImageView &get_vulkan_data() const;
};
}
}
}
}
#endif
