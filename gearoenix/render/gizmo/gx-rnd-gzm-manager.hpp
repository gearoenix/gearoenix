#ifndef GEAROENIX_RENDER_GIZMO_MANAGER_HPP
#define GEAROENIX_RENDER_GIZMO_MANAGER_HPP
#include "../../math/gx-math-matrix-4d.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::gizmo {
struct Drawer;
struct Manager final {
    engine::Engine& e;
    camera::Camera* current_camera = nullptr;
    boost::container::flat_multimap<float, int> axes_z_map;

private:
    math::Mat4x4<float> current_view_matrix;
    math::Mat4x4<float> current_projection_matrix;
    boost::container::flat_set<Drawer*> drawers;

public:
    explicit Manager(engine::Engine& e);
    ~Manager();
    void show_view();
    void set_viewport_camera(camera::Camera*);
    [[nodiscard]] bool show_transform(math::Mat4x4<double>&) const;
    [[nodiscard]] bool is_processing_inputs() const;
    void register_drawer(Drawer*);
    void remove_drawer(Drawer*);
};
}
#endif
