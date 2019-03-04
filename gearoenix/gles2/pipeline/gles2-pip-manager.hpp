#ifndef GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/pipeline/rnd-pip-manager.hpp"
#include <memory>
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace pipeline {
        class Manager : public render::pipeline::Manager {
        private:
        public:
            std::shared_ptr<render::pipeline::Resource> create_resource(const std::vector<std::shared_ptr<render::texture::Texture> > &ts) = 0;
        };
    }
}
}
#endif
#endif
