#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "rnd-txt-texture-2d.hpp"
namespace gearoenix::render::texture {
class Target : public Texture2D {
protected:
    core::Real clipping_width = 0.0f;
    core::Real clipping_height = 0.0f;

    Target(const core::Id my_id, engine::Engine* const e) noexcept
        : Texture2D(my_id, e, Type::Target2D)
    {
    }

public:
    virtual ~Target() noexcept = default;
    void set_clipping(core::Real w, core::Real h) noexcept
    {
        clipping_width = w;
        clipping_height = h;
    }
};
}
#endif
