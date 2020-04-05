#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class TextureCube : public Texture {
    GX_GET_VAL_PRT(std::size_t, aspect, 0)
protected:
    TextureCube(
        core::Id id,
        TextureFormat texture_format,
        engine::Engine* engine) noexcept;

public:
    ~TextureCube() noexcept override;

    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept override;
};
}
#endif
