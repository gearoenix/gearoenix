#ifndef GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
#define GEAROENIX_VULKAN_PIPELINE_MANAGER_HPP
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
    namespace descriptor {
        class Manager;
    }
    class RenderPass;
    namespace pipeline {
        class Cache;
        class Pipeline;
        class Manager {
        private:
            Cache* cache;
            RenderPass* rndpass;
            core::cache::Cacher* cacher;
            descriptor::Manager* desmgr;
            Engine* engine;

        public:
            Manager(Engine* engine);
            ~Manager();
            std::shared_ptr<Pipeline> get_pipeline(shader::Id sid);
            std::shared_ptr<Pipeline> get_cached_pipeline(shader::Id sid);
        };
    }
}
}
#endif
