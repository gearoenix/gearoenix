#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cr-static.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
#include <memory>

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::shader {
class Shader;
}

namespace gearoenix::render::pipeline {
class ResourceSet;
class Pipeline {
    GX_GET_CVAL_PRT(Type, pipeline_type)
    GX_GET_REFC_PRT(std::string, name)
protected:
    engine::Engine* const e;

    Pipeline(const Type pipeline_type, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
        : pipeline_type(pipeline_type)
        , name(std::to_string(pipeline_type))
        , e(e)
    {
    }

public:
    virtual ~Pipeline() noexcept = default;
    virtual ResourceSet* create_resource_set() const noexcept = 0;
};
}

#endif
