#ifndef GEAROENIX_RENDER_GIZMO_DRAWER_HPP
#define GEAROENIX_RENDER_GIZMO_DRAWER_HPP

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::gizmo {
struct Manager;
struct Drawer {
    Manager* const gizmo_manager;
    bool is_gizmo_visible = false;

    explicit Drawer(engine::Engine* e);
    virtual ~Drawer();
    virtual void draw_gizmo() = 0;
};
}
#endif
