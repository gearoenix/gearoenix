#ifndef GEAROENIX_RENDER_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
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
    GX_GET_CVAL_PRV(Id, id);
    GX_GET_REFC_PRV(std::string, name);
    GX_GETSET_VAL_PRT(bool, is_shadow_caster, true);
    GX_GETSET_VAL_PRT(bool, is_shadow_receiver, true);
    GX_GETSET_VAL_PRT(Transparency, transparency, Transparency::Opaque);

protected:
    explicit Material(std::string name, Id id) noexcept;

public:
    virtual ~Material() noexcept;
    Material(const Material&) noexcept = delete;
    Material(Material&&) noexcept = delete;
    /// This function was needed in several other places that needs to set albedo no matter what material type we have
    /// On the other hand, most of the materials have an albedo texture.
    virtual void set_albedo(const std::shared_ptr<texture::Texture2D>&) noexcept;
};
}
#endif