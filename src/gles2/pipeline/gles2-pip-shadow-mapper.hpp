#ifndef GEAROEMIX_GLES2_PIPELINE_SHADOW_MAPPER_HPP
#define GEAROEMIX_GLES2_PIPELINE_SHADOW_MAPPER_HPP
#include "../../render/pipeline/rnd-pip-shadow-mapper.hpp"
namespace gearoenix {
namespace gles2 {
    namespace engine {
        class Engine;
    }
    namespace shader {
        class ShadowMapper;
    }
    namespace pipeline {
        class ShadowMapper : public render::pipeline::ShadowMapper {
        private:
            const std::shared_ptr<shader::ShadowMapper> shd;

        public:
            ShadowMapper(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
            render::pipeline::ResourceSet* create_resource_set() const override final;
        };
    }
}
}
#endif
