#ifndef GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
#define GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../core/gc/cr-gc-object.hpp"
namespace gearoenix {
namespace render {
    namespace command {
        class Buffer;
    }
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
            void copy_from(command::Buffer* cb, SubBuffer* sb);
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif
#endif // GEAROENIX_VULKAN_BUFFER_SUB_BUFFER_HPP
