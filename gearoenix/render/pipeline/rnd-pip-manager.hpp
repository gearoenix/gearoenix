#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "rnd-pip-pipeline.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
namespace gearoenix {
namespace core {
    namespace cache {
        class Cacher;
    }
}
namespace render {
    class Engine;
    namespace pipeline {
        class Pipeline;
        class Manager {
        private:
            Engine* eng;

        public:
            Manager(Engine* engine);
            ~Manager();
            std::shared_ptr<Pipeline> get(PipelineType::Id pipeline_type, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
