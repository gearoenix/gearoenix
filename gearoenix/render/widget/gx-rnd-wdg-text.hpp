#pragma once
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-wdg-alignment.hpp"
#include "gx-rnd-wdg-widget.hpp"

namespace gearoenix::render::font {
struct Font;
}

namespace gearoenix::render::material {
struct Unlit;
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

namespace gearoenix::render::widget {
struct Text final : Widget {
    struct ConstructorReturn final {
        std::shared_ptr<Text> text;
        std::shared_ptr<model::Builder> model_builder;
    };

    GX_GETSET_CREF_PRT(std::wstring, text);
    GX_GETSET_CREF_PRT(std::shared_ptr<font::Font>, text_font);
    GX_GETSET_CREF_PRT(math::Vec4<double>, text_colour);
    GX_GETSET_VAL_PRT(Alignment, vertical_align, Alignment::Center);
    GX_GETSET_VAL_PRT(Alignment, horizontal_align, Alignment::Center);
    GX_GET_CREF_PRT(std::shared_ptr<material::Unlit>, text_material);
    GX_GET_CREF_PRT(std::weak_ptr<Text>, text_self);

private:
    Text(std::string&& name, engine::Engine& e);

public:
    static void construct(
        std::string&& name,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<ConstructorReturn>&& end_callback);
    static void construct(
        std::string&& name,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<material::Unlit>&& mat,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<ConstructorReturn>&& end_callback);
    static void construct(
        std::string&& name,
        core::ecs::entity_id_t camera_id,
        std::shared_ptr<mesh::Mesh>&& msh,
        std::shared_ptr<Widget>&& parent,
        std::shared_ptr<scene::Builder>&& scene_builder,
        core::job::EndCaller<ConstructorReturn>&& end_callback);
    ~Text() override;
    [[nodiscard]] math::Vec2<double> get_text_size() const;
    void update_text(const core::job::EndCaller<>& c) const;
};
}