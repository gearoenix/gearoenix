#include "gx-rnd-mat-unlit.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Unlit::Unlit(const core::object_type_index_t final_type_index, std::string&& name)
    : Material(final_type_index, std::move(name), true)
    , albedo_factor(1.0f, 1.0f, 1.0f, 1.0f)
{
}

void gearoenix::render::material::Unlit::initialise(core::job::EndCallerShared<Unlit>&& c)
{
    texture::Manager::get().create_2d_from_colour(
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
