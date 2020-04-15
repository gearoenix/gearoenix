#include "rnd-mat-material.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include "rnd-mat-pbr.hpp"
#include "rnd-mat-unlit.hpp"

gearoenix::render::material::Material::Material(const Type t, engine::Engine* const e, const std::size_t uniform_size) noexcept
    : material_type(t)
    , uniform_buffers(new buffer::FramedUniform(uniform_size, e))
    , e(e)
{
}

float gearoenix::render::material::Material::read_alpha(system::stream::Stream* const f) noexcept
{
    if (f->read_bool()) {
        return 1.0f;
    } else {
        return f->read<float>();
    }
}

std::pair<std::shared_ptr<gearoenix::render::texture::Texture2D>, std::optional<gearoenix::math::Vec4<float>>>
gearoenix::render::material::Material::read_color(
    system::stream::Stream* const f,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept
{
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> txt_call([end](const std::shared_ptr<texture::Texture>&) {});
        return std::make_pair(std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call)), std::nullopt);
    } else {
        core::sync::EndCaller<texture::Texture2D> txt_call([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec4<float> color;
        color.read(f);
        return std::make_pair(std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_2d(color, txt_call)), color);
    }
}

std::pair<std::shared_ptr<gearoenix::render::texture::Texture2D>, std::optional<gearoenix::math::Vec3<float>>>
gearoenix::render::material::Material::read_emission(
    system::stream::Stream* const f,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept
{
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> txt_call([end](const std::shared_ptr<texture::Texture>&) {});
        return std::make_pair(std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call)), std::nullopt);
    } else {
        core::sync::EndCaller<texture::Texture2D> txt_call([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec3<float> color;
        color.read(f);
        return std::make_pair(std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_2d(color, txt_call)), color);
    }
}

gearoenix::render::material::Material::~Material() noexcept = default;

gearoenix::render::material::Material* gearoenix::render::material::Material::read(
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    const auto t = f->read<Type>();
    switch (t) {
    case Type::Pbr:
        return new Pbr(f, e, end);
    case Type::Unlit:
        return new Unlit(f, e, end);
    default:
        GXUNEXPECTED
    }
}

std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>>
gearoenix::render::material::Material::read_emission(gearoenix::system::stream::Stream* f,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& end) const noexcept
{
    return std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>>();
}

std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>>
gearoenix::render::material::Material::read_emission(gearoenix::system::stream::Stream* f,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& end) const noexcept
{
    return std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>>();
}

std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>
gearoenix::render::material::Material::read_color(gearoenix::system::stream::Stream* f) const noexcept
{
    return std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>();
}

std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>
gearoenix::render::material::Material::read_color(gearoenix::system::stream::Stream* f) const noexcept
{
    return std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>();
}

std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>
gearoenix::render::material::Material::read_color(gearoenix::system::stream::Stream* f) const noexcept
{
    return std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>>();
}
