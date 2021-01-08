#include "gx-rnd-txt-texture-2d.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

gearoenix::render::texture::Texture2D::Texture2D(
    const core::Id id,
    std::string name,
    const TextureFormat texture_format,
    const SamplerInfo& sampler_info,
    engine::Engine* const e) noexcept
    : Texture(id, std::move(name), Type::Texture2D, texture_format, sampler_info, e)
{
}

gearoenix::render::texture::Texture2D::~Texture2D() noexcept = default;

void gearoenix::render::texture::Texture2D::write_gx3d(
    const std::shared_ptr<platform::stream::Stream>& s,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    Texture::write_gx3d(s, c);
    (void)s->write(static_cast<std::uint16_t>(img_width));
    (void)s->write(static_cast<std::uint16_t>(img_height));
}
