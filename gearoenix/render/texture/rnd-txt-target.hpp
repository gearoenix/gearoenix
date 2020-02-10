#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "../../core/cr-static.hpp"
#include "rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
class Target : virtual public Texture {
    GX_GETSET_VAL_PRT(core::Real, clipping_width, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_height, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_starting_x, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_starting_y, 0.0f)
protected:
    Target(const Type t, const core::Id my_id, engine::Engine* const e) noexcept
        : Texture(my_id, t, e)
    {
    }

public:
    ~Target() noexcept override = default;
    [[nodiscard]] virtual Target* clone() const noexcept = 0;
};
}
#endif
