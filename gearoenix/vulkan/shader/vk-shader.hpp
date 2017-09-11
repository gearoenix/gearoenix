#ifndef GEAROENIX_VULKAN_SHADER_SHADER_HPP
#define GEAROENIX_VULKAN_SHADER_SHADER_HPP
#include "../vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace device {
        class Logical;
    }
    namespace shader {
        class Shader {
        protected:
            std::shared_ptr<device::Logical> logical_device;
            void delete_module(const VkShaderModule& module);
            void fetch(std::shared_ptr<system::File>& file, std::vector<uint8_t>& data);
            VkShaderModule load(std::shared_ptr<system::File>& file);

        public:
            Shader(const std::shared_ptr<device::Logical>& logical_device);
            virtual ~Shader();
            virtual VkShaderModule get_module(int index) = 0;
        };
    } // namespace shader
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_SHADER_HPP
