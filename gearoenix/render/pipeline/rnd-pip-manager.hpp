#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
#include <vector>

namespace gearoenix {
namespace render {
    class Engine;
    namespace texture {
        class Texture;
    }
    namespace pipeline {
        class Pipeline;
        class Resource;
        class Manager {
        protected:
            Engine* e;
            core::cache::Cacher<Pipeline> pipelines;

            Manager(Engine* engine);

        public:
            virtual ~Manager();
            virtual std::shared_ptr<Pipeline> get(const Type::Id pipeline_type, core::sync::EndCaller<core::sync::EndCallerIgnore> end) = 0;
            virtual std::shared_ptr<Resource> create_resource(const std::vector<std::shared_ptr<texture::Texture>>& ts) = 0;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
