#ifndef GEAROENIX_VULKAN_INSTANCE_HPP
#define GEAROENIX_VULKAN_INSTANCE_HPP
#include "vk-linker.hpp"
#include <memory>
#include <set>
#include "../core/cr-build-configuration.hpp"
#ifdef DEBUG_MODE
#define VULKAN_INSTANCE_DEBUG
#endif
namespace gearoenix {
namespace render {
class Instance {
private:
	std::shared_ptr<Linker> linker;
	VkInstance vulkan_data;
#ifdef VULKAN_INSTANCE_DEBUG
	VkDebugReportCallbackEXT report_callback;
#endif
public:
	Instance(const std::shared_ptr<Linker> &l);
	~Instance();
	const std::shared_ptr<Linker> &get_linker() const;
	const VkInstance &get_vulkan_data() const;
};
}
}
#endif
