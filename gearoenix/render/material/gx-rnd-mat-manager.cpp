#include "gx-rnd-mat-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-mat-pbr.hpp"
#include "gx-rnd-mat-sprite.hpp"
#include "gx-rnd-mat-unlit.hpp"

gearoenix::render::material::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::material::Manager::~Manager() = default;

#define GX_RND_MAT_RETURN_IF_FOUND(x)                                        \
    {                                                                        \
        const std::lock_guard _lg(materials_lock);                           \
        if (auto search = materials.find(name); materials.end() != search) { \
            if (auto m = search->second.lock()) {                            \
                GX_ASSERT_D(Id::x == m->get_id());                           \
                auto cm = std::dynamic_pointer_cast<x>(m);                   \
                GX_ASSERT_D(nullptr != cm);                                  \
                c.set_return(std::move(cm));                                 \
                return;                                                      \
            }                                                                \
        }                                                                    \
    }                                                                        \
    static_assert(true, "")

#define GX_RND_MAT_CONSTRUCT(x, sx)                                                                             \
    GX_RND_MAT_RETURN_IF_FOUND(x);                                                                              \
    construct_##sx(name, core::job::EndCallerShared<x>([this, c = std::move(c), name](std::shared_ptr<x>&& m) { \
        const std::lock_guard _lg(materials_lock);                                                              \
        materials.emplace(name, m);                                                                             \
        c.set_return(std::move(m));                                                                             \
    }))

#define GX_RND_MAT_FUNC(x, sx)                                      \
    void gearoenix::render::material::Manager::get_##sx(            \
        const std::string& name, core::job::EndCallerShared<x>&& c) \
    {                                                               \
        GX_RND_MAT_CONSTRUCT(x, sx);                                \
    }                                                               \
    static_assert(true, "")

GX_RND_MAT_FUNC(Pbr, pbr);
GX_RND_MAT_FUNC(Unlit, unlit);
GX_RND_MAT_FUNC(Sprite, sprite);
