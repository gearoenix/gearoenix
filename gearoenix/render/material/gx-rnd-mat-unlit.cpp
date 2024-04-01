#include "gx-rnd-mat-unlit.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Unlit::Unlit(engine::Engine& e, const std::string& name)
    : Material(e, name, Id::Unlit)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
{
}

void gearoenix::render::material::Unlit::initialise(core::job::EndCallerShared<Unlit>&& c)
{
    const auto& tm = e.get_texture_manager();
    tm->create_2d_from_colour(
        math::Vec4(1.0f, 1.0f, 1.0f, 1.0f),
        core::job::EndCallerShared<texture::Texture2D>([this, c](std::shared_ptr<texture::Texture2D>&& t) {
            set_albedo(std::move(t));
            (void)c;
        }));
}

gearoenix::render::material::Unlit::~Unlit() = default;

void gearoenix::render::material::Unlit::set_albedo(std::shared_ptr<texture::Texture2D>&& t)
{
    albedo = t;
}
