#ifndef GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#define GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
#include "../../core/gc/cr-gc.hpp"
#include "../vk-linker.hpp"
namespace gearoenix {
namespace render {
    namespace device {
        class Logical;
    }
    namespace memory {
        class Memory;
        class SubMemory;
        class Manager : public core::gc::Gc {
        public:
            typedef enum : unsigned int {
                GPU_LOCAL,
                CPU_COHERENT,
            } Place;

        private:
            Memory* mem;
            VkMemoryRequirements mem_reqs;

        public:
            Manager(device::Logical* logical_device, const VkMemoryRequirements& mem_reqs, const Place& place = GPU_LOCAL);
            ~Manager();
            Memory* get_memory();
            const Memory* get_memory() const;
            SubMemory* create_submemory(unsigned int size);
        };
    } // namespace memory
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_MEMORY_MANAGER_HPP
