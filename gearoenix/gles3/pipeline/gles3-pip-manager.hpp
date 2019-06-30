#ifndef GEAROENIX_GLES3_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES3_PIPELINE_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../render/pipeline/rnd-pip-manager.hpp"
namespace gearoenix::gles3 {
namespace engine {
    class Engine;
}
namespace pipeline {
    class Pipeline;
    class Manager : public render::pipeline::Manager {
    public:
        Manager(const std::shared_ptr<engine::Engine>& engine);
        ~Manager() override final;
        std::shared_ptr<render::pipeline::Pipeline> get(const render::pipeline::Type::Id pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) override final;
    };
}
}
#endif
#endif