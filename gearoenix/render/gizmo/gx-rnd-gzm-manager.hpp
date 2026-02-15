#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::gizmo {
struct Drawer;
struct Manager final : core::Singleton<Manager> {
    camera::Camera* current_camera = nullptr;
    boost::container::flat_multimap<float, int> axes_z_map;

private:
    math::Mat4x4<float> current_view_matrix;
    math::Mat4x4<float> current_projection_matrix;
    math::Mat4x4<float> current_view_projection_matrix;
    math::Vec3<float> current_camera_position;
    boost::container::flat_set<Drawer*> drawers;
    std::uint64_t operation_handles = 0;
    std::uint64_t transform_mode = 0;
    std::uintptr_t active_handle = static_cast<std::uintptr_t>(-1);

public:
    Manager();
    ~Manager() override;
    void show_view();
    void set_viewport_camera(camera::Camera*);
    [[nodiscard]] bool show_transform(math::Mat4x4<double>&) const;
    [[nodiscard]] bool show(math::Aabb3<double>&);
    [[nodiscard]] bool is_processing_inputs() const;
    void register_drawer(Drawer*);
    void remove_drawer(Drawer*);
    void enable_translation_handle();
    void disable_translation_handle();
    void enable_rotation_handle();
    void disable_rotation_handle();
    void enable_scale_handle();
    void disable_scale_handle();
    void enable_local_transform_mode();
    void disable_local_transform_mode();
    [[nodiscard]] bool draw_translate_handle(math::Vec3<double>& point, const math::Vec2<float>& projected_point, std::uintptr_t pointer_id);
};
}