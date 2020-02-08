#ifndef GEAROENIX_RENDER_TEXTURE_TARGET_2D_HPP
#define GEAROENIX_RENDER_TEXTURE_TARGET_2D_HPP
#include "rnd-txt-target.hpp"
#include "rnd-txt-texture-2d.hpp"

namespace gearoenix::render::texture {
class Target2D : public Texture2D, public Target {
protected:
    Target2D(const core::Id my_id, engine::Engine* const e) noexcept
        : Texture(my_id, Type::Target2D, e)
        , Texture2D(my_id, e)
        , Target(Type::Target2D, my_id, e)
    {
    }

public:
    virtual ~Target2D() noexcept = default;
};
}
#endif
