#include "gx-rnd-txt-texture-cube.hpp"
#include "../../system/stream/gx-sys-stm-stream.hpp"

gearoenix::render::texture::TextureCube::TextureCube(
    const core::Id id,
    std::string name,
    const TextureFormat texture_format,
    const SamplerInfo& sampler_info,
    engine::Engine* const engine) noexcept
    : Texture(id, std::move(name), Type::TextureCube, texture_format, sampler_info, engine)
{
}

gearoenix::render::texture::TextureCube::~TextureCube() noexcept = default;

void gearoenix::render::texture::TextureCube::write_gx3d(
    const std::shared_ptr<system::stream::Stream>& s,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    Texture::write_gx3d(s, c);
    (void)s->write(static_cast<std::uint16_t>(aspect));
}