#include "gx-rnd-mat-unlit.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Unlit::Unlit(engine::Engine& e, const std::string& name, const core::sync::EndCaller& c) noexcept
    : Material(name, Id::Unlit)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
{
    const auto& tm = e.get_texture_manager();
    albedo = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
}

gearoenix::render::material::Unlit::~Unlit() noexcept = default;

void gearoenix::render::material::Unlit::set_albedo(const std::shared_ptr<texture::Texture2D>& t) noexcept
{
    albedo = t;
}