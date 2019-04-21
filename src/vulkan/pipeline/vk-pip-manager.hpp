#ifndef GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
#define GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../render/shader/rnd-shd-shader.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    namespace cache {
        class Cacher;
    }
}
namespace render {
    class Engine;
    class RenderPass;
    namespace descriptor {
        class Pool;
    }
    namespace pipeline {
        class Cache;
        class Pipeline;
        class Manager {
        private:
            Cache* cache;
            RenderPass* rndpass;
            descriptor::Pool* despool;
            core::cache::Cacher* cacher;
            Engine* engine;

        public:
            Manager(Engine* engine);
            ~Manager();
            std::shared_ptr<Pipeline> get_pipeline(shader::Id sid);
            std::shared_ptr<Pipeline> get_cached_pipeline(shader::Id sid);
            descriptor::Pool* get_descriptor_pool();
        };
    }
}
}
#endif
#endif
