#include "gx-rnd-mat-material.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../buffer/gx-rnd-buf-framed-uniform.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-mat-pbr.hpp"
#include "gx-rnd-mat-unlit.hpp"

gearoenix::render::material::Material::Material(const Type t, engine::Engine* const e, const std::size_t uniform_size) noexcept
    : material_type(t)
    , uniform_buffers(new buffer::FramedUniform(uniform_size, e))
    , e(e)
{
}

gearoenix::render::material::Material::Material(const Material& o) noexcept
    : material_type(o.material_type)
    , uniform_buffers(new buffer::FramedUniform(o.uniform_buffers->get_buffer()->get_size(), o.e))
    , e(o.e)
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
gearoenix::render::material::Material::read_t2d_v4(
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
gearoenix::render::material::Material::read_t2d_v3(
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

std::pair<std::shared_ptr<gearoenix::render::texture::Texture2D>, std::optional<gearoenix::math::Vec2<float>>>
gearoenix::render::material::Material::read_t2d_v2(
    system::stream::Stream* const f,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept
{
    auto* const txt_mgr = e->get_system_application()->get_asset_manager()->get_texture_manager();
    if (f->read_bool()) {
        core::sync::EndCaller<texture::Texture> txt_call([end](const std::shared_ptr<texture::Texture>&) {});
        return std::make_pair(std::dynamic_pointer_cast<texture::Texture2D>(txt_mgr->get_gx3d(f->read<core::Id>(), txt_call)), std::nullopt);
    } else {
        core::sync::EndCaller<texture::Texture2D> txt_call([end](const std::shared_ptr<texture::Texture2D>&) {});
        math::Vec2<float> color;
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
        GX_UNEXPECTED
    }
}

gearoenix::render::material::Material* gearoenix::render::material::Material::clone() const noexcept
{
    GX_UNIMPLEMENTED
}
