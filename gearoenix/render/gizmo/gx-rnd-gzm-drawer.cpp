#include "gx-rnd-gzm-drawer.hpp"
#include "gx-rnd-gzm-manager.hpp"

gearoenix::render::gizmo::Drawer::Drawer()
{
    Manager::get().register_drawer(this);
}

gearoenix::render::gizmo::Drawer::~Drawer()
{
    Manager::get().remove_drawer(this);
}
