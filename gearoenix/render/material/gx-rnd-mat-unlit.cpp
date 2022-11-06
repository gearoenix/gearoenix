#include "gx-rnd-mat-unlit.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::material::Unlit::Unlit(engine::Engine& e, const core::sync::EndCallerIgnored& c) noexcept
    : Material(Id::Unlit)
    , albedo_factor(1.0f)
{
    const auto& tm = e.get_texture_manager();
    albedo = tm->create_2d_from_colour(math::Vec4(1.0f, 1.0f, 1.0f, 1.0f), c);
}

gearoenix::render::material::Unlit::~Unlit() noexcept = default;