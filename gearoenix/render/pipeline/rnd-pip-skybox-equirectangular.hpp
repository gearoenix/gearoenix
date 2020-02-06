#ifndef GEAROENIX_RENDER_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#define GEAROENIX_RENDER_PIPELINE_SKYBOX_EQUIRECTANGULAR_HPP
#include "rnd-pip-pipeline.hpp"
namespace gearoenix::render::pipeline {
class SkyboxEquirectangular : public Pipeline {
protected:
    SkyboxEquirectangular(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
        : Pipeline(Type::SkyboxEquirectangular, e, call)
    {
    }

public:
    ~SkyboxEquirectangular() noexcept override = default;
};
}
#endif
