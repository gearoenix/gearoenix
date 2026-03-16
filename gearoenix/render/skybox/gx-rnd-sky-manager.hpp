#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Path;
struct Stream;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Texture;
}

namespace gearoenix::render::skybox {
struct Manager : core::Singleton<Manager> {
protected:
    Manager();

public:
    ~Manager() override;

    [[nodiscard]] virtual core::ecs::EntityPtr build(std::string&& name, core::ecs::Entity* parent, std::shared_ptr<texture::Texture>&& bound_texture, std::shared_ptr<mesh::Mesh>&& bound_mesh) = 0;

    void build(std::string&& name, core::ecs::Entity* parent, const platform::stream::Path& texture_path, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);

    void build(std::string&& name, core::ecs::Entity* parent, std::shared_ptr<texture::Texture>&& bound_texture, core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback);
};
}