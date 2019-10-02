#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "rnd-txt-type.hpp"
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace texture {
    class Texture : public core::asset::Asset {
        GX_GET_CVAL_PRT(Type, texture_type)
    protected:
        engine::Engine* const render_engine;
        Texture(const core::Id my_id, const Type texture_type, engine::Engine* const e) noexcept
            : core::asset::Asset(my_id, core::asset::Type::Texture)
            , texture_type(texture_type)
            , render_engine(e)
        {
        }

    public:
        virtual ~Texture() noexcept = default;
    };
}
}
#endif
