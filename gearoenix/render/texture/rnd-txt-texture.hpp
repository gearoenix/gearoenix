#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
namespace gearoenix::render {
namespace engine {
    class Engine;
}
namespace texture {
    class Texture : public core::asset::Asset {
    protected:
        engine::Engine* const render_engine;
        Texture(core::Id my_id, engine::Engine* e) noexcept
            : core::asset::Asset(my_id, core::asset::Type::TEXTURE)
            , render_engine(e)
        {
        }

    public:
        virtual ~Texture() noexcept = default;
    };
}
}
#endif
