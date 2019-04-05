#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
#include <vector>

namespace gearoenix {
namespace render {
namespace buffer {
class Uniform;
}
    namespace engine {
        class Engine;
    }
    namespace texture {
        class Texture;
    }
    namespace pipeline {
        class Pipeline;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
            core::cache::Cacher<Pipeline, Type::Id> pipelines;
            Manager(const std::shared_ptr<engine::Engine>& e);
        public:
            virtual ~Manager();
            virtual std::shared_ptr<Pipeline> get(const Type::Id pipeline_type, core::sync::EndCaller<Pipeline> &end) = 0;
        };
    }
}
}
#endif
