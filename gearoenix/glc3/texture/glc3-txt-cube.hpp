#ifndef GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#define GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-texture-cube.hpp"
#include "../../render/texture/rnd-txt-texture-info.hpp"

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::texture {
class TextureCube final : public render::texture::TextureCube {
    GX_GET_VAL_PRV(gl::uint, texture_object, 0)
private:
    TextureCube(
        core::Id id,
        render::texture::TextureFormat texture_format,
        const render::texture::SampleInfo& sample_info,
        engine::Engine* engine) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<TextureCube> construct(
        core::Id id,
        engine::Engine* engine,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::size_t aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~TextureCube() noexcept final;
    void write_gx3d(
        const std::shared_ptr<system::stream::Stream>& s,
        const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>&) noexcept final;
    void bind(gl::enumerated texture_unit) const noexcept;
    void bind() const noexcept;
    void generate_mipmap() const noexcept;
};
}

#endif
#endif
