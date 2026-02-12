#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-object-type-indices.hpp"
#include "../gx-gl-types.hpp"
#include <array>

namespace gearoenix::gl {
struct Texture2D;
}

namespace gearoenix::gl {
struct Camera;
struct Model;
struct Mesh;
struct Scene;
}

namespace gearoenix::render::record {
struct Camera;
struct CameraModel;
struct Model;
}

namespace gearoenix::gl::material {
struct Material {
    constexpr static auto max_count = 8;
    constexpr static auto object_type_index = gearoenix_gapi_material_type_index;
    constexpr static std::array<core::object_type_index_t, 0> all_parent_object_type_indices {};
    constexpr static std::array<core::object_type_index_t, 0> immediate_parent_object_type_indices {};

    Material();
    virtual ~Material();
    virtual void shadow(
        const Mesh&, const render::record::Camera&, const render::record::CameraModel&, uint& current_shader);
    virtual void render_forward(
        const Scene&, const render::record::Camera&, const render::record::CameraModel&, const Mesh&, uint& current_shader);
    virtual void deferred_gbuffer_render(const Model&, const Mesh&, const Camera&, const Scene&, uint& current_shader);
};
}

#endif