#pragma once
#include "../../core/gx-cr-object.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-mat-transparency.hpp"
#include <memory>
#include <string>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::material {
struct Material : core::Object {
    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_render_material_type_index;

    GX_GETSET_VAL_PRT(bool, is_shadow_caster, true);
    GX_GETSET_VAL_PRT(bool, is_shadow_receiver, true);
    GX_GETSET_VAL_PRT(Transparency, transparency, Transparency::Opaque);
    GX_GET_CVAL_PRT(bool, need_model_view_projection_matrix);

    Material(core::object_type_index_t final_object_type, std::string&& name, bool need_model_view_projection_matrix);

public:
    ~Material() override;
    Material(const Material&) = delete;
    Material(Material&&) = delete;

    /// This function was needed in several other places that need to set albedo no matter what material type we have.
    /// On the other hand, most of the materials have an albedo texture.
    virtual void set_albedo(std::shared_ptr<texture::Texture2D>&&);
    void show_debug_gui() override;
};
}