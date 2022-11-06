#include "gx-rnd-mat-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-mat-pbr.hpp"
#include "gx-rnd-mat-unlit.hpp"

gearoenix::render::material::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::material::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::material::Pbr> gearoenix::render::material::Manager::get_pbr(
    const std::string& name, const core::sync::EndCallerIgnored& c) noexcept
{
    if (auto search = materials.find(name); materials.end() != search) {
        if (auto m = search->second.lock()) {
            GX_ASSERT_D(Id::Pbr == m->get_id());
            return std::dynamic_pointer_cast<Pbr>(m);
        }
    }
    auto m = construct_pbr(name, c);
    materials.emplace(name, m);
    return m;
}

std::shared_ptr<gearoenix::render::material::Unlit> gearoenix::render::material::Manager::get_unlit(
    const std::string& name, const core::sync::EndCallerIgnored& c) noexcept
{
    if (auto search = materials.find(name); materials.end() != search) {
        if (auto m = search->second.lock()) {
            GX_ASSERT_D(Id::Unlit == m->get_id());
            return std::dynamic_pointer_cast<Unlit>(m);
        }
    }
    auto m = construct_unlit(name, c);
    materials.emplace(name, m);
    return m;
}