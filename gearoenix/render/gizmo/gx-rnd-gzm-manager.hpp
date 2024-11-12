#ifndef GEAROENIX_RENDER_GIZMO_MANAGER_HPP
#define GEAROENIX_RENDER_GIZMO_MANAGER_HPP
#include <boost/container/flat_map.hpp>

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::gizmo {
struct Manager final {
    engine::Engine& e;
    camera::Camera* current_camera = nullptr;
    boost::container::flat_multimap<float, int> axes_z_map;

private:
public:
    explicit Manager(engine::Engine& e);
    void set_viewport_camera(camera::Camera*);
    ~Manager();
    void show_view();
};
}
#endif
