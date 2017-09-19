#ifndef GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#define GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
#include "../../core/gc/cr-gc-object.hpp"
namespace gearoenix {
namespace render {
    namespace memory {
        class SubMemory : public core::gc::Object {
        private:
        public:
            SubMemory(unsigned int size);
            ~SubMemory();
        };
    } // namespace memory
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_MEMORY_SUB_MEMORY_HPP
