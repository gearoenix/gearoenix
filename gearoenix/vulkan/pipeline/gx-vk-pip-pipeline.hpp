#ifndef GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#define GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../core/cache/gx-cr-cache-cached.hpp"
#include "../../render/shader/gx-rnd-shd-shader.hpp"
#include "../gx-vk-linker.hpp"
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
            Engine* eng;

        public:
            Pipeline(shader::Id sid,
                Cache* cache,
                RenderPass* rndpass,
                const std::shared_ptr<shader::Shader>& shd,
                descriptor::SetLayout* dessetlay,
                Engine* eng);
            ~Pipeline();
            const VkPipeline& get_vulkan_data() const;
            descriptor::SetLayout* get_descriptor_set_layout();
            const Layout* get_layout() const;
            void bind();
        };
    }
}
}
#endif
#endif
