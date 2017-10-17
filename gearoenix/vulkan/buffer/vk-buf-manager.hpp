#ifndef GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#define GEAROENIX_VULKAN_BUFFER_MANAGER_HPP
#include "../../core/gc/cr-gc.hpp"
namespace gearoenix {
namespace render {
    namespace memory {
        class Manager;
    }
    namespace buffer {
        class Buffer;
        class SubBuffer;
        class Manager : public core::gc::Gc {
        public:
            typedef enum : unsigned int {
                VERTEX = 1,
                INDEX = 2,
                UNIFORM = 4,
            } Usage;

        private:
            memory::Manager* mem_mgr;
            Buffer* buff;
            Usage usage;
            unsigned int align, coalign, decoalign;
            Manager* parent = nullptr;

        public:
            Manager(memory::Manager* mem_mgr, unsigned int size, Usage usage = (Usage)(VERTEX | INDEX | UNIFORM));
            Manager(unsigned int size, Manager* parent);
            ~Manager();
            const Buffer* get_buffer() const;
            Buffer* get_buffer();
            SubBuffer* create_subbuffer(unsigned int size);
        };
    }
}
}
#endif
