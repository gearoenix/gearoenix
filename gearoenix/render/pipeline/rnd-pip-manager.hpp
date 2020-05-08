#ifndef GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#define GEAROENIX_RENDER_PIPELINE_MANAGER_HPP
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-pip-type.hpp"
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace pipeline {
    class Pipeline;
    class Manager {
    protected:
        engine::Engine* const e;
        core::cache::Cacher<Pipeline, Type> pipelines;
        Manager(engine::Engine* const e) noexcept
            : e(e)
        {
        }

    public:
        virtual ~Manager() noexcept = default;
        virtual std::shared_ptr<Pipeline> get(Type pipeline_type, core::sync::EndCaller<Pipeline>& end) noexcept = 0;
    };
}
}
#endif