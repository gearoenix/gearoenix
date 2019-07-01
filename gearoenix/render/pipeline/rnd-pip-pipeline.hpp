#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
#include <memory>
namespace gearoenix::render {
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
        engine::Engine* const e;
        const Type::Id pipeline_type;
        Pipeline(const Type::Id pipeline_type, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
            : e(e)
            , pipeline_type(pipeline_type)
        {
        }

    public:
        virtual ~Pipeline() noexcept = default;

        Type::Id get_pipeline_type_id() const noexcept
        {
            return pipeline_type;
        }

        virtual ResourceSet* create_resource_set() const noexcept = 0;
    };
}
}
#endif
