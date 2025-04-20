#pragma once

namespace gearoenix::render::gizmo {
struct Manager;
struct Drawer {
    bool is_gizmo_visible = false;

    Drawer();
    virtual ~Drawer();
    virtual void draw_gizmo() = 0;
};
}