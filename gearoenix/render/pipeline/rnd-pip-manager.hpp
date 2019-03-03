#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "rnd-pip-pipeline.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <vector>
#include <map>

namespace gearoenix {
namespace core {
    namespace cache {
        class Cacher;
    }
}
namespace render {
    class Engine;
	namespace texture {
		class Texture;
	}
    namespace pipeline {
        class Pipeline;
		class Resource;
        class Manager {
        private:
            Engine* eng;
			std::map<PipelineType::Id, std::weak_ptr<Pipeline> > pipelines;

        public:
            Manager(Engine* engine);
            ~Manager();
            std::shared_ptr<Pipeline> get(PipelineType::Id pipeline_type, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
			virtual std::shared_ptr<Resource> create_resource(const std::vector<std::shared_ptr<texture::Texture> > &ts) = 0;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
