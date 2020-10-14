#ifndef GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#define GEAROENIX_RENDER_TEXTURE_CUBE_HPP
#include "gx-rnd-txt-texture.hpp"
namespace gearoenix::render::texture {
class TextureCube : public Texture {
    GX_GET_VAL_PRT(std::size_t, aspect, 0)
protected:
    TextureCube(
        core::Id id,
        std::string name,
        TextureFormat texture_format,
        const SamplerInfo& sample_info,
        engine::Engine* engine) noexcept;

public:
    ~TextureCube() noexcept override;

    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept override;
};
}
#endif
