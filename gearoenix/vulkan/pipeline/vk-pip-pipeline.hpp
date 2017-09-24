#ifndef GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
#define GEAROENIX_VULKAN_PIPELINE_PIPELINE_HPP
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
        class Set;
    }
    class RenderPass;
    namespace pipeline {
        class Cache;
        class Layout;
        class Pipeline {
        private:
            shader::Id sid;
            device::Logical* dev;
            Cache* cache;
            Layout* layout;
            RenderPass* rndpass;
            std::shared_ptr<shader::Shader> shd;
            std::shared_ptr<descriptor::Set> desset;
            VkPipeline vulkan_data;

        public:
            Pipeline(shader::Id sid,
                Cache* cache,
                RenderPass* rndpass,
                const std::shared_ptr<shader::Shader>& shd,
                const std::shared_ptr<descriptor::Set>& desset);
            ~Pipeline();
            const VkPipeline& get_vulkan_data() const;
        };
    }
}
}
#endif
