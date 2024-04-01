#ifndef GEAROENIX_RENDER_WIDGET_LABEL_HPP
#define GEAROENIX_RENDER_WIDGET_LABEL_HPP
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::model {
struct Builder;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::widget {
struct Label final : Widget {
    Label(std::string&& name, engine::Engine& e);
    ~Label() override;
    static void construct(
        std::string&& name,
        std::string&& background_texture_asset,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Label>>>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Label>>>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        std::shared_ptr<material::Material>&& mat,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Label>>>&& end_callback);
    static void construct(
        std::string&& name,
        std::shared_ptr<texture::Texture2D>&& background_texture,
        std::shared_ptr<material::Material>&& mat,
        std::shared_ptr<mesh::Mesh>&& msh,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<std::pair<std::shared_ptr<model::Builder>, std::shared_ptr<Label>>>&& end_callback);
};
}
#endif
