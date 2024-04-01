#ifndef GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#define GEAROENIX_RENDER_SKYBOX_MANAGER_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-sky-types.hpp"
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::skybox {
struct Builder;
struct Manager {

protected:
    engine::Engine& e;

    explicit Manager(engine::Engine& e);

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    virtual ~Manager();

    [[nodiscard]] virtual std::shared_ptr<Builder> build(
        std::string&& name,
        Texture&& bound_texture,
        std::shared_ptr<mesh::Mesh>&& bound_mesh,
        core::job::EndCaller<>&& entity_end_callback)
        = 0;

    void build(
        std::string&& name,
        platform::stream::Path&& texture_path,
        core::job::EndCaller<>&& entity_end_callback,
        core::job::EndCallerShared<Builder>&& builder_callback);

    void build(
        std::string&& name,
        Texture&& bound_texture,
        core::job::EndCaller<>&& entity_end_callback,
        core::job::EndCallerShared<Builder>&& builder_callback);
};
}
#endif
