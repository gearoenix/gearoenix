#ifndef GEAROENIX_GLC3_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLC3_PIPELINE_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../render/pipeline/rnd-pip-manager.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::pipeline {
class Pipeline;
class Manager : public render::pipeline::Manager {
public:
    explicit Manager(engine::Engine* engine) noexcept;
    ~Manager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::pipeline::Pipeline> get(render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept final;
};
}
#endif
#endif