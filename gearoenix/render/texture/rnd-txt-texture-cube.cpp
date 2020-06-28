#include "rnd-txt-texture-cube.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::texture::TextureCube::TextureCube(
    const core::Id id,
    std::string name,
    const TextureFormat texture_format,
    const SampleInfo& sample_info,
    engine::Engine* const engine) noexcept
    : Texture(id, std::move(name), Type::TextureCube, texture_format, sample_info, engine)
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