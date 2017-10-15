#ifndef GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#define GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#include "../../core/cache/cr-cache-cached.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../vk-linker.hpp"
namespace gearoenix {
namespace core {
    class Application;
}
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class SetLayout;
    }
    class RenderPass;
    namespace pipeline {
        class Cache;
        class Layout;
        class Pipeline : public core::cache::Cached {
        private:
            shader::Id sid;
            device::Logical* dev;
            Cache* cache;
            descriptor::SetLayout* dessetlay;
            Layout* layout;
            RenderPass* rndpass;
            std::shared_ptr<shader::Shader> shd;
            VkPipeline vulkan_data;

        public:
            Pipeline(shader::Id sid,
                Cache* cache,
                RenderPass* rndpass,
                const std::shared_ptr<shader::Shader>& shd,
                descriptor::SetLayout* dessetlay);
            ~Pipeline();
            const VkPipeline& get_vulkan_data() const;
            descriptor::SetLayout* get_descriptor_set_layout();
        };
    }
}
}
#endif
