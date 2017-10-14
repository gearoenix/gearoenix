#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../vk-linker.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace render {
    namespace command {
        class Pool;
        class Buffer {
        private:
            Pool* pool;
            Linker* linker;
            VkCommandBuffer vulkan_data;
            bool not_flushed = false;

        public:
            Buffer(Pool* pool);
            ~Buffer();
            void begin();
            void reset();
            void copy_buffer(VkBuffer src, VkBuffer des, const VkBufferCopy& region);
            void flush();
            const VkCommandBuffer& get_vulkan_data() const;
            void begin_render_pass_with_info(
                const VkRenderPassBeginInfo& render_pass_begin_info);
            void set_viewport(const VkViewport& viewport);
            void set_scissor(const VkRect2D& scissor);
            void end();
            void end_render_pass();
        };
    }
}
}
#endif
