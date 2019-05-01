#ifndef GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#define GEAROENIX_VULKAN_COMMAND_BUFFER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
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
            void bind_pipeline(VkPipeline pip);
            void bind_descriptor_set(
                VkPipelineBindPoint pipeline_bind_point,
                VkPipelineLayout pipeline_layout,
                uint32_t first_set,
                const VkDescriptorSet& desset);
            void bind_vertex_buffers(VkBuffer buf, VkDeviceSize offset);
            void bind_index_buffer(VkBuffer buf, VkDeviceSize offset);
            void draw_indexed(VkDeviceSize count);
        };
    }
}
}
#endif
#endif
