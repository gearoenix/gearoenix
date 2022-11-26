#include "gx-rnd-mat-sprite.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Sprite::Sprite(engine::Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept
    : Material(name, Id::Sprite)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
    , uv_transform(1.0f, 1.0f, 0.0f, 0.0f)
{
    const auto& tm = e.get_texture_manager();
    albedo = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
}

gearoenix::render::material::Sprite::~Sprite() noexcept = default;

void gearoenix::render::material::Sprite::set_albedo(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    albedo = t;
}