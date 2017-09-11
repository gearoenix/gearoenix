#ifndef GEAROENIX_NUFRAG_VULKAN_SHADER_DIFFUSE_COLORED_HPP
#define GEAROENIX_NUFRAG_VULKAN_SHADER_DIFFUSE_COLORED_HPP
#define DIFFUSE_COLORED_MODULE_COUNT 2
#include "vk-shader.hpp"
namespace gearoenix {
namespace nufrag {
namespace render {
namespace shader {
class DiffuseColored: public Shader {
private:
    VkShaderModule vertex;
    VkShaderModule fragment;
public:
    DiffuseColored(const std::shared_ptr<device::Logical> &logical_device, std::shared_ptr<system::File> &file);
    ~DiffuseColored();
    VkShaderModule get_module(int index);
};
} // namespace shader
} // namespace render
} // namespace nufrag
} // namespace gearoenix
#endif
