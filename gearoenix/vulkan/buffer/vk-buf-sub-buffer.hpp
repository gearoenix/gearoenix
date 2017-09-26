#ifndef GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
#include "../../core/gc/cr-gc-object.hpp"
namespace gearoenix {
namespace render {
    namespace buffer {
        class Buffer;
        class SubBuffer : public core::gc::Object {
        private:
            Buffer* buff;

        public:
            SubBuffer(unsigned int size, Buffer* buff);
            ~SubBuffer();
            const Buffer* get_buffer() const;
            void write(const void* data, unsigned int data_size);
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
