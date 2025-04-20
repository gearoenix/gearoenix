#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::widget {
struct Label final : Widget {
    Label(std::string&& name);
    ~Label() override;
    static void construct(
        std::string&& name,
        std::string&& background_texture_asset,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Label>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Label>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        std::shared_ptr<material::Material>&& mat,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Label>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        std::shared_ptr<material::Material>&& mat,
        std::shared_ptr<mesh::Mesh>&& msh,
        core::ecs::Entity* camera_entity,
        std::shared_ptr<Widget>&& parent,
        core::job::EndCallerShared<Label>&& end_callback);
};
}