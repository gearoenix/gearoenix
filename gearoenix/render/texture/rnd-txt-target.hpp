#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_HPP
#include "rnd-txt-texture.hpp"

namespace gearoenix::render::texture {
class Target : virtual public Texture {
protected:
    Target(const Type t, const core::Id my_id, engine::Engine* const e) noexcept
        : Texture(my_id, t, e)
    {
    }

public:
    ~Target() noexcept override = default;
};
}
#endif
