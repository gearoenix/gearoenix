#ifndef GEAROENIX_RENDER_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-mat-id.hpp"
#include "gx-rnd-mat-transparency.hpp"
#include <memory>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::material {
struct Material {
    GX_GETSET_VAL_PRT(bool, is_shadow_caster, true);
    GX_GETSET_VAL_PRT(bool, is_shadow_receiver, true);
    GX_GET_CVAL_PRV(Id, id);
    GX_GETSET_VAL_PRT(Transparency, transparency, Transparency::Opaque);

protected:
    explicit Material(Id id) noexcept;

public:
    virtual ~Material() noexcept;
    Material(const Material&) noexcept = delete;
    Material(Material&&) noexcept = delete;
};
}
#endif