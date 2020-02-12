#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "../../core/cr-static.hpp"
#include "rnd-txt-type.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class Texture;
class Target {
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_CVAL_PRT(Type, target_type)
    GX_GET_CVAL_PRT(core::Id, target_id)
    GX_GETSET_VAL_PRT(core::Real, clipping_width, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_height, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_starting_x, 0.0f)
    GX_GETSET_VAL_PRT(core::Real, clipping_starting_y, 0.0f)
protected:
    Target(const Type t, const core::Id id, engine::Engine* const e) noexcept
        : e(e)
        , target_type(t)
        , target_id(id)
    {
    }

public:
    virtual ~Target() noexcept = default;
    [[nodiscard]] virtual const Texture* get_texture(std::size_t index) const noexcept = 0;
    [[nodiscard]] virtual std::size_t get_textures_count() const noexcept = 0;
    [[nodiscard]] virtual Target* clone() const noexcept = 0;
};
}
#endif
