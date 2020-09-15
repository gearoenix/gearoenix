#ifndef GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#define GEAROENIX_GLES2_PIPELINE_MANAGER_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/cache/gx-cr-cache-cacher.hpp"
#include "../../render/pipeline/gx-rnd-pip-manager.hpp"

namespace gearoenix::gles2::engine {
class Engine;
}

namespace gearoenix::gles2::pipeline {
class Pipeline;
class Manager final : public render::pipeline::Manager {
public:
    explicit Manager(engine::Engine* engine) noexcept;
    ~Manager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::pipeline::Pipeline> get(render::pipeline::Type pipeline_type_id, core::sync::EndCaller<render::pipeline::Pipeline>& end) noexcept final;
};
}

#endif
#endif