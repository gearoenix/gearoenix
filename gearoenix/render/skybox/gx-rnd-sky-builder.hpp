#ifndef GEAROENIX_RENDER_SKYBOX_BUILDER_HPP
#define GEAROENIX_RENDER_SKYBOX_BUILDER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::skybox {
struct Builder {
    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);

    Builder(
        engine::Engine& e,
        std::string&& name,
        core::job::EndCaller<>&& entity_end_callback);

public:
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    virtual ~Builder();
};
}
#endif
