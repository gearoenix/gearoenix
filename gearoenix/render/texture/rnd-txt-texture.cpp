#include "rnd-txt-texture.hpp"
#include "../../system/stream/sys-stm-local.hpp"

gearoenix::render::texture::Texture::Texture(
    const core::Id my_id,
    const Type texture_type,
    const TextureFormat texture_format,
    engine::Engine* const e) noexcept
    : core::asset::Asset(my_id, core::asset::Type::Texture)
    , texture_type(texture_type)
    , texture_format(texture_format)
    , render_engine(e)
{
}

gearoenix::render::texture::Texture::~Texture() noexcept = default;

void gearoenix::render::texture::Texture::write_gx3d(
    const std::string& file_address,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
{
    std::shared_ptr<system::stream::Stream> l(new system::stream::Local(file_address, true));
    write_gx3d(l, c);
}

void gearoenix::render::texture::Texture::write_gx3d(
    const std::shared_ptr<system::stream::Stream>& s,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
{
    (void)s->write(texture_type);
    (void)s->write(texture_format);
}
