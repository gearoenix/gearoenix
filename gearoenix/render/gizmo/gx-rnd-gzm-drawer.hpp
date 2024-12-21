#pragma once

namespace gearoenix::render::gizmo {
struct Manager;
struct Drawer {
    Manager* const gizmo_manager;
    bool is_gizmo_visible = false;

    Drawer();
    virtual ~Drawer();
    virtual void draw_gizmo() = 0;
};
}