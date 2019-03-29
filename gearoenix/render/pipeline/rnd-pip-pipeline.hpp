#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
#include <memory>
namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
    namespace shader {
        class Shader;
    }
    namespace pipeline {
        class ResourceSet;
        class Pipeline {
        protected:
            const std::shared_ptr<engine::Engine> e;
            const Type::Id pipeline_type;

        public:
            Pipeline(const Type::Id pipeline_type, const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &call);
            virtual ~Pipeline();
            Type::Id get_pipeline_type_id() const;
            virtual ResourceSet* create_resource_set() const = 0;
        };
    }
}
}
#endif