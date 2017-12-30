#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include <memory>
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
            core::cache::Cacher* csh;

        public:
            Manager(Engine* engine);
            ~Manager();
            std::shared_ptr<Pipeline> get_pipeline(core::Id sid, core::EndCaller<core::EndCallerIgnore> end);
            std::shared_ptr<Pipeline> get_cached_pipeline(core::Id sid) const;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
