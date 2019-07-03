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
    protected:
        engine::Engine* const render_engine;
		const Type::Id texture_type;
        Texture(const core::Id my_id, const Type::Id texture_type, engine::Engine*const e) noexcept
            : core::asset::Asset(my_id, core::asset::Type::TEXTURE)
            , render_engine(e)
			, texture_type(texture_type)
        {
        }

    public:
        virtual ~Texture() noexcept = default;
		Type::Id get_texture_type() const noexcept { return texture_type; }
    };
}
}
#endif
