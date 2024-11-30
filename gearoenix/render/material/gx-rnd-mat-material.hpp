#pragma once
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-mat-id.hpp"
#include "gx-rnd-mat-transparency.hpp"
#include <memory>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::material {
struct Material {
    engine::Engine& e;
    GX_GET_CVAL_PRV(Id, id);
    GX_GET_REFC_PRV(std::string, name);
    GX_GETSET_VAL_PRT(bool, is_shadow_caster, true);
    GX_GETSET_VAL_PRT(bool, is_shadow_receiver, true);
    GX_GETSET_VAL_PRT(Transparency, transparency, Transparency::Opaque);

    Material(engine::Engine& e, std::string name, Id id);

public:
    virtual ~Material();
    Material(const Material&) = delete;
    Material(Material&&) = delete;
    /// This function was needed in several other places that needs to set albedo no matter what material type we have
    /// On the other hand, most of the materials have an albedo texture.
    virtual void set_albedo(std::shared_ptr<texture::Texture2D>&&);
};
}