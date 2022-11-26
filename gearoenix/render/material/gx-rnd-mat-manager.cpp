#include "gx-rnd-mat-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-mat-pbr.hpp"
#include "gx-rnd-mat-sprite.hpp"
#include "gx-rnd-mat-unlit.hpp"

gearoenix::render::material::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::material::Manager::~Manager() noexcept = default;

#define GX_RND_MAT_RETURN_IF_FOUND(x)                                    \
    if (auto search = materials.find(name); materials.end() != search) { \
        if (auto m = search->second.lock()) {                            \
            GX_ASSERT_D(Id::x == m->get_id());                           \
            return std::dynamic_pointer_cast<x>(m);                      \
        }                                                                \
    }                                                                    \
    static_assert(true, "")

std::shared_ptr<gearoenix::render::material::Pbr> gearoenix::render::material::Manager::get_pbr(
    const std::string& name, const core::sync::EndCaller& c) noexcept
{
    GX_RND_MAT_RETURN_IF_FOUND(Pbr);
    auto m = construct_pbr(name, c);
    materials.emplace(name, m);
    return m;
}

std::shared_ptr<gearoenix::render::material::Unlit> gearoenix::render::material::Manager::get_unlit(
    const std::string& name, const core::sync::EndCaller& c) noexcept
{
    GX_RND_MAT_RETURN_IF_FOUND(Unlit);
    auto m = construct_unlit(name, c);
    materials.emplace(name, m);
    return m;
}

std::shared_ptr<gearoenix::render::material::Sprite> gearoenix::render::material::Manager::get_sprite(
    const std::string& name, const core::sync::EndCaller& c) noexcept
{
    GX_RND_MAT_RETURN_IF_FOUND(Sprite);
    auto m = construct_sprite(name, c);
    materials.emplace(name, m);
    return m;
}
