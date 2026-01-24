#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "gx-rnd-mat-material.hpp"

namespace gearoenix::render::material {
struct Unlit : Material {
    constexpr static auto max_count = 256;
    constexpr static auto object_type_index = gearoenix_render_material_unlit_type_index;
    constexpr static std::array all_parent_object_type_indices { Material::object_type_index };
    constexpr static std::array immediate_parent_object_type_indices { Material::object_type_index };

    GX_GET_REF_PRT(math::Vec4<float>, albedo_factor);
    GX_GET_REF_PRT(math::Vec4<float>, uv_transform);
    GX_GET_VAL_PRT(float, alpha_cutoff, 0.0);
    GX_GET_CREF_PRT(std::shared_ptr<texture::Texture2D>, albedo);

    Unlit(core::object_type_index_t final_type_index, std::string&& name);
    void initialise(core::job::EndCallerShared<Unlit>&& c);

public:
    ~Unlit() override;
    void set_albedo(std::shared_ptr<texture::Texture2D>&& t) override;
    virtual void set_albedo_factor(const math::Vec4<float>&);
    virtual void set_uv_transform(const math::Vec4<float>&);
    virtual void set_alpha_cutoff(float);
};
}