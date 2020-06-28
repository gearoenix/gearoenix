#include "rnd-txt-texture-2d.hpp"
#include "../../system/stream/sys-stm-stream.hpp"

gearoenix::render::texture::Texture2D::Texture2D(
    const core::Id id,
    std::string name,
    const TextureFormat texture_format,
    const SampleInfo& sample_info,
    engine::Engine* const e) noexcept
    : Texture(id, std::move(name), Type::Texture2D, texture_format, sample_info, e)
{
}

gearoenix::render::texture::Texture2D::~Texture2D() noexcept = default;

void gearoenix::render::texture::Texture2D::write_gx3d(
    const std::shared_ptr<system::stream::Stream>& s,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    Texture::write_gx3d(s, c);
    (void)s->write(static_cast<std::uint16_t>(img_width));
    (void)s->write(static_cast<std::uint16_t>(img_height));
}
