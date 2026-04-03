#include "gx-rnd-mat-sprite.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Sprite::Sprite(const core::object_type_index_t final_type_index, std::string&& name)
    : Material(final_type_index, std::move(name), true)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
    , uv_transform(1.0f, 1.0f, 0.0f, 0.0f)
{
}

void gearoenix::render::material::Sprite::initialise(core::job::EndCallerShared<Sprite>&& c)
{
    texture::Manager::get().create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c = std::move(c)](std::shared_ptr<texture::Texture2D>&& t) {
            set_albedo(std::move(t));
        }));
}

gearoenix::render::material::Sprite::~Sprite() = default;

void gearoenix::render::material::Sprite::set_albedo(std::shared_ptr<texture::Texture2D>&& t)
{
    albedo = std::move(t);
}

void gearoenix::render::material::Sprite::set_albedo_factor(const math::Vec4<float>& v)
{
    albedo_factor = v;
}

void gearoenix::render::material::Sprite::set_uv_transform(const math::Vec4<float>& v)
{
    uv_transform = v;
}

void gearoenix::render::material::Sprite::set_alpha_cutoff(const float v)
{
    alpha_cutoff = v;
}
