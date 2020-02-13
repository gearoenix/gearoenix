#ifndef GEAROENIX_GLC3_PIPELINE_UNLIT_HPP
#define GEAROENIX_GLC3_PIPELINE_UNLIT_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/pipeline/rnd-pip-unlit.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::shader {
class Unlit;
}

namespace gearoenix::glc3::pipeline {
class Unlit : public render::pipeline::Unlit {
private:
    const std::shared_ptr<shader::Unlit> shd;
    std::weak_ptr<Unlit> self;
    Unlit(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Unlit> construct(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Unlit() noexcept final;
    [[nodiscard]] render::pipeline::ResourceSet* create_resource_set() const noexcept final;
};
}

#endif
#endif
