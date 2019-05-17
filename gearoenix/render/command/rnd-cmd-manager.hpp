#ifndef GEAROENIX_RENDER_COMMAND_MANAGER_HPP
#define GEAROENIX_RENDER_COMMAND_MANAGER_HPP

namespace gearoenix {
namespace render {
    namespace command {
        class Buffer;
        class Manager {
        public:
            virtual Buffer* create_primary_command_buffer() = 0;
            virtual Buffer* create_secondary_command_buffer() = 0;
            virtual Buffer* create_primary_command_buffer(const unsigned int kernel_index) = 0;
            virtual Buffer* create_secondary_command_buffer(const unsigned int kernel_index) = 0;
        };
    }
}
}

#endif