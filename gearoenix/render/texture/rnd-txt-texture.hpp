#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "rnd-txt-format.hpp"
#include "rnd-txt-type.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class Texture : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, texture_type)
    GX_GET_CVAL_PRT(TextureFormat, texture_format)
    GX_GET_CPTR_PRT(engine::Engine, render_engine)
protected:
    Texture(
        const core::Id my_id,
        const Type texture_type,
        const TextureFormat texture_format,
        engine::Engine* const e) noexcept
        : core::asset::Asset(my_id, core::asset::Type::Texture)
        , texture_type(texture_type)
        , texture_format(texture_format)
        , render_engine(e)
    {
    }

public:
    ~Texture() noexcept override = default;
};
}

#endif
