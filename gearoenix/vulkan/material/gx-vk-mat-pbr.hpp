#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/material/gx-rnd-mat-pbr.hpp"
#include "gx-vk-mat-material.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::material {
struct Pbr final : render::material::Pbr, Material {
    GEAROENIX_OBJECT_STRUCT_DEF;

    constexpr static auto max_count = render::material::Pbr::max_count;
    constexpr static auto object_type_index = gearoenix_gapi_material_pbr_type_index;
    constexpr static std::array all_parent_object_type_indices {
        render::material::Material::object_type_index,
        render::material::Pbr::object_type_index,
        material::Material::object_type_index
    };
    constexpr static std::array immediate_parent_object_type_indices {
        render::material::Pbr::object_type_index,
        material::Material::object_type_index
    };

private:
    explicit Pbr(std::string&& name);

public:
    static void construct(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c);
    ~Pbr() override;
    void set(bool skinned, DrawCache& dc) override;
};
}
#endif