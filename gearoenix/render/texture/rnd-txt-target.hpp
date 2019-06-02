#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "rnd-txt-texture-2d.hpp"
namespace gearoenix::render::texture {
class Target : public Texture2D {
protected:
    Target(const core::Id my_id, engine::Engine* const e) noexcept
        : Texture2D(my_id, e)
    {
    }

public:
    virtual ~Target() noexcept = default;
};
}
#endif
