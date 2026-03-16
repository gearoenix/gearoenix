#pragma once
#include "../../core/gx-cr-object.hpp"

namespace gearoenix::render::material {
struct Material;
}

namespace gearoenix::render::mesh {
struct Buffer;
struct Mesh : core::Object {
    constexpr static auto max_count = 8192;
    constexpr static auto object_type_index = gearoenix_render_mesh_type_index;

    GX_GET_CREF_PRT(std::shared_ptr<Buffer>, buffer);
    GX_GET_CREF_PRT(std::shared_ptr<material::Material>, bound_material);

    Mesh(std::string&& name, std::shared_ptr<Buffer>&& buffer, std::shared_ptr<material::Material>&& material);

public:
    ~Mesh() override;

    Mesh(Mesh&&) = delete;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    virtual void set_material(std::shared_ptr<material::Material>&& material);
    void show_debug_gui() override;
};
}