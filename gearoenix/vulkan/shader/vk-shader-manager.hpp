#ifndef GEAROENIX_VULKAN_SHADER_MANAGER_HPP
#define GEAROENIX_VULKAN_SHADER_MANAGER_HPP
#include <map>
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace device {
        class Logical;
    }
    namespace shader {
        class Shader;
        class Manager {
        private:
            std::map<uint16_t, std::weak_ptr<Shader>> cached;
            std::map<uint16_t, uint32_t> offsets;
            std::shared_ptr<system::File> asset_file;

        public:
            Manager(std::shared_ptr<system::File> file);
            std::shared_ptr<Shader> get_shader(
                const std::shared_ptr<device::Logical>& logical_device, uint16_t id);
        };
    } // namespace shader
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_SHADER_MANAGER_HPP
