#ifndef GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#define GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#include <memory>
#include <string>
#include "../vk-linker.hpp"
namespace gearoenix {
namespace core {
class Application;
}
namespace render {
namespace device {
class Logical;
}
namespace shader {
class DiffuseColored;
class Manager;
class Shader;
}
class RenderPass;
class Swapchain;
namespace pipeline {
class Cache;
class Layout;
class Pipeline {
private:
    std::shared_ptr<Cache> cache;
    std::shared_ptr<Layout> layout;
    std::shared_ptr<RenderPass> render_pass;
    std::shared_ptr<shader::DiffuseColored> diffuse;
	VkPipeline vulkan_data;
public:
	enum ShaderType {
		VERTEX_SHADER, FRAGMENT_SHADER
	};
    Pipeline(
            const std::shared_ptr<Cache> &cache,
            const std::shared_ptr<Layout> &layout,
            const std::shared_ptr<RenderPass> &render_pass,
            std::shared_ptr<shader::Manager> &shader_manager);
    ~Pipeline();
	const VkPipeline &get_vulkan_data() const;
};
}
}
}
#endif
