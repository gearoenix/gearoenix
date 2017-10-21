#ifndef GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../core/gc/cr-gc-object.hpp"
namespace gearoenix {
namespace render {
    namespace memory {
        class Memory;
        class SubMemory : public core::gc::Object {
        private:
            Memory* mem;

        public:
            SubMemory(unsigned int size, Memory* mem);
            ~SubMemory();
            const Memory* get_memory() const;
        };
    } // namespace memory
} // namespace render
} // namespace gearoenix
#endif
#endif // GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
