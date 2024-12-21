#include "gx-rnd-gzm-drawer.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-gzm-manager.hpp"

gearoenix::render::gizmo::Drawer::Drawer()
    : gizmo_manager(engine::Engine::get() ? engine::Engine::get()->get_gizmo_manager() : nullptr)
{
    if (gizmo_manager) {
        gizmo_manager->register_drawer(this);
    }
}

gearoenix::render::gizmo::Drawer::~Drawer()
{
    if (gizmo_manager) {
        gizmo_manager->remove_drawer(this);
    }
}
