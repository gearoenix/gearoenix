#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../math/gx-math-matrix-4d.hpp"
#include "../../math/gx-math-vector-4d.hpp"
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
    /// Viewable rect (x, y, w, h) for gizmos — usually set by the editor to the free
    /// 3D area (below the menu bar, to the left of the right panel). A width or
    /// height of 0 means "use the full display", which is the default before anyone
    /// calls `set_viewport_rect`.
    math::Vec4<float> viewport_rect { 0.0f, 0.0f, 0.0f, 0.0f };

    /// Resolve `viewport_rect` against the fallback (full `io.DisplaySize`) into a
    /// concrete `{x, y, w, h}` in screen coordinates. Returns zero-width/height on
    /// degenerate displays.
    [[nodiscard]] math::Vec4<float> resolved_viewport_rect() const;

public:
    Manager();
    ~Manager() override;
    void show_view();
    void set_viewport_camera(camera::Camera*);
    /// Tell the manager where the 3D viewport lives in screen coordinates. Passing
    /// width or height == 0 means "fall back to the full display".
    void set_viewport_rect(float x, float y, float w, float h) { viewport_rect = { x, y, w, h }; }
    [[nodiscard]] bool show_transform(math::Mat4x4<core::fp_t>&) const;
    [[nodiscard]] bool show(math::Aabb3<core::fp_t>&);
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
    [[nodiscard]] bool draw_translate_handle(math::Vec3<core::fp_t>& point, const math::Vec2<float>& projected_point, std::uintptr_t pointer_id);
};
}