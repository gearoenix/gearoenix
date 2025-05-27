#include "gx-rnd-gzm-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include <boost/functional/hash.hpp>
#include <imgui.h>
#include <tuple>
// Keep it after imgui
#include <ImGuizmo/ImGuizmo.h>

namespace {
gearoenix::render::gizmo::Manager* instance = nullptr;
}

gearoenix::render::gizmo::Manager::Manager()
{
    enable_translation_handle();
    disable_local_transform_mode();
    GX_ASSERT_D(!instance);
    instance = this;
}

gearoenix::render::gizmo::Manager::~Manager()
{
    GX_ASSERT_D(instance == this);
    instance = nullptr;
}

gearoenix::render::gizmo::Manager& gearoenix::render::gizmo::Manager::get()
{
    static Manager m;
    return m;
}

void gearoenix::render::gizmo::Manager::show_view()
{
    if (nullptr == current_camera) {
        return;
    }
    const auto& trn = *current_camera->get_transform();

    const auto& io = ImGui::GetIO();

    constexpr auto width = 100.0f;
    constexpr auto margin = 20.0f;
    constexpr auto top_margin = 40.0f;
    constexpr auto radius = 10.0f;

    constexpr auto half_width = width * 0.5f;
    constexpr auto center_x = half_width + margin;
    constexpr auto center_y = half_width + top_margin;

    const math::Vec3 center(io.DisplaySize.x - center_x, center_y, 0.0f);
    const auto im_center = ImVec2(center.x, center.y);

    const auto view = current_camera->get_view();
    std::array<math::Vec3<float>, 7> axes;

    const auto z_dis_origin = (current_camera->get_near() + current_camera->get_far()) * -0.5f;

    axes[0] = math::Vec3<float>(trn.get_local_position() + trn.get_z_axis() * z_dis_origin);
    axes[1] = axes[0] + math::X3D<float>;
    axes[2] = axes[0] - math::X3D<float>;
    axes[3] = axes[0] + math::Y3D<float>;
    axes[4] = axes[0] - math::Y3D<float>;
    axes[5] = axes[0] + math::Z3D<float>;
    axes[6] = axes[0] - math::Z3D<float>;

    const auto axes_scale = half_width / static_cast<float>(trn.get_scale().maximum());

    for (auto& axis : axes) {
        axis = (view * math::Vec4(axis, 0.0f)).xyz() * axes_scale;
    }

    axes[0] -= center;
    axes_z_map.clear();
    for (int i = 1; i < axes.size(); ++i) {
        axes[i] -= axes[0];
        axes_z_map.emplace(axes[i].z, i);
        axes[i].y = center.y * 2.0f - axes[i].y;
    }

    auto* const dl = ImGui::GetWindowDrawList();

    int less_i = 0;
    for (const auto [_, i] : axes_z_map) {
        if (++less_i > 3) {
            break;
        }
        dl->AddCircle({ axes[i].x, axes[i].y }, radius, IM_COL32(0, 0, 0, 255) | (50 << (8 * ((i - 1) >> 1))), 16, 5.0f);
    }

    less_i = 0;
    for (const auto [_, i] : axes_z_map) {
        const auto zl = ++less_i > 3;
        dl->AddLine(im_center, { axes[i].x, axes[i].y }, IM_COL32(0, 0, 0, 255) | ((zl ? 200 : 50) << (8 * ((i - 1) >> 1))), 5.0f);
    }

    less_i = 0;
    for (const auto [_, i] : axes_z_map) {
        if (++less_i < 4) {
            continue;
        }
        dl->AddCircleFilled({ axes[i].x, axes[i].y }, radius, IM_COL32(0, 0, 0, 255) | (200 << (8 * ((i - 1) >> 1))), 16);
        dl->AddText(nullptr, 20, { axes[i].x - radius * 0.9f, axes[i].y - radius }, IM_COL32_BLACK, std::array { "+X", "-X", "+Y", "-Y", "+Z", "-Z" }[i - 1]);
    }
}

void gearoenix::render::gizmo::Manager::set_viewport_camera(camera::Camera* const c)
{
    current_camera = c;
    if (nullptr == current_camera) {
        return;
    }
    current_view_matrix = c->get_view();
    current_projection_matrix = c->get_projection();
    current_view_projection_matrix = c->get_view_projection();
    current_camera_position = math::Vec3<float>(c->get_transform()->get_global_position());

    ImGuizmo::SetOrthographic(c->is_orthographic());
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    for (auto* const d : drawers) {
        if (!d->is_gizmo_visible) {
            continue;
        }
        ImGuizmo::PushID(d);
        d->draw_gizmo();
        ImGuizmo::PopID();
    }
}

bool gearoenix::render::gizmo::Manager::show_transform(math::Mat4x4<double>& inout) const
{
    math::Mat4x4<float> m(inout);
    if (!Manipulate(
            current_view_matrix.data(),
            current_projection_matrix.data(),
            static_cast<ImGuizmo::OPERATION>(operation_handles),
            static_cast<ImGuizmo::MODE>(transform_mode),
            m.data())) {
        return false;
    }
    inout = math::Mat4x4<double>(m);
    return true;
}

bool gearoenix::render::gizmo::Manager::show(math::Aabb3<double>& box)
{
    std::array<math::Vec3<double>, 8> points;
    box.get_all_corners(points);
    bool result = false;
    std::array<math::Vec2<float>, points.size()> display_points;
    const auto& io = ImGui::GetIO();
    const auto mid_screen = math::Vec2(io.DisplaySize.x, io.DisplaySize.y) * 0.5f;
    const auto draw_scale = math::Vec2(mid_screen.x, -mid_screen.y);
    for (auto i = 0; i < points.size(); ++i) {
        const auto p = current_view_projection_matrix * math::Vec4(math::Vec3<float>(points[i]), 1.0f);
        const auto pp = p.xyz() / p.w;
        if (pp.z > 1.0 || pp.z < -1.0) {
            return false;
        }
        display_points[i] = (pp.xy() * draw_scale) + mid_screen;
    }
    auto* const dl = ImGui::GetWindowDrawList();
    const auto box_center = math::Vec3<float>(box.get_center());
    const auto box_radius = math::Vec3<float>(box.get_diameter()) * 0.5f;
    constexpr std::array<math::Vec3<float>, 6> unit_directions { { { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f } } };
    std::array<bool, 6> backs {};
    for (auto i = 0; i < unit_directions.size(); ++i) {
        const auto& direction = unit_directions[i];
        const auto dir = (box_center + (direction * box_radius)) - current_camera_position;
        backs[i] = dir.dot(direction) > 0;
    }
    const std::array<std::tuple<int, int, bool>, 12> index_pair_normals { { { 0, 1, backs[0] && backs[4] },
        { 1, 3, backs[0] && backs[3] },
        { 3, 2, backs[0] && backs[5] },
        { 2, 0, backs[0] && backs[2] },
        { 0, 4, backs[2] && backs[4] },
        { 1, 5, backs[3] && backs[4] },
        { 2, 6, backs[2] && backs[5] },
        { 3, 7, backs[3] && backs[5] },
        { 4, 5, backs[1] && backs[4] },
        { 5, 7, backs[1] && backs[3] },
        { 7, 6, backs[1] && backs[5] },
        { 6, 4, backs[1] && backs[2] } } };
    for (const auto& [fi, si, back] : index_pair_normals) {
        const auto& fp = display_points[fi];
        const auto& sp = display_points[si];
        dl->AddLine(
            reinterpret_cast<const ImVec2&>(fp), reinterpret_cast<const ImVec2&>(sp),
            back ? IM_COL32(200, 200, 100, 128) : IM_COL32(250, 250, 140, 128),
            back ? 1.0f : 3.5f);
    }
    auto hash = boost::hash_value(__FUNCTION__);
    boost::hash_combine(hash, boost::hash_value(reinterpret_cast<std::uintptr_t>(&box)));
    for (auto i = 0; i < display_points.size(); ++i) {
        auto point_hash = hash;
        boost::hash_combine(point_hash, boost::hash_value(i));
        const auto& p = display_points[i];
        if (draw_translate_handle(points[i], p, point_hash)) {
            result = true;
            box.reset(points[i]);
            box.put(points[math::Aabb3<double>::counter_corners_indices[i]]);
        }
    }
    return result;
}

bool gearoenix::render::gizmo::Manager::is_processing_inputs() const
{
    return ImGuizmo::IsUsingAny();
}

void gearoenix::render::gizmo::Manager::register_drawer(Drawer* const d)
{
    drawers.emplace(d);
}

void gearoenix::render::gizmo::Manager::remove_drawer(Drawer* const d)
{
    drawers.erase(d);
}

void gearoenix::render::gizmo::Manager::enable_translation_handle()
{
    operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::TRANSLATE);
}

void gearoenix::render::gizmo::Manager::disable_translation_handle()
{
    operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::TRANSLATE);
}

void gearoenix::render::gizmo::Manager::enable_rotation_handle()
{
    operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::ROTATE);
}

void gearoenix::render::gizmo::Manager::disable_rotation_handle()
{
    operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::ROTATE);
}

void gearoenix::render::gizmo::Manager::enable_scale_handle()
{
    operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::SCALE);
}

void gearoenix::render::gizmo::Manager::disable_scale_handle()
{
    operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::SCALE);
}

void gearoenix::render::gizmo::Manager::enable_local_transform_mode()
{
    transform_mode = ImGuizmo::MODE::LOCAL;
}

void gearoenix::render::gizmo::Manager::disable_local_transform_mode()
{
    transform_mode = ImGuizmo::MODE::WORLD;
}

bool gearoenix::render::gizmo::Manager::draw_translate_handle(
    math::Vec3<double>& point, const math::Vec2<float>& projected_point, std::uintptr_t pointer_id)
{
    constexpr auto radius = 5.0f;
    constexpr auto colour = IM_COL32(250, 140, 140, 128);
    const auto& io = ImGui::GetIO();
    const math::Vec2 mouse_pos(io.MousePos.x, io.MousePos.y);
    auto* const dl = ImGui::GetWindowDrawList();
    if ((mouse_pos - projected_point).square_length() < radius * radius) {
        dl->AddCircleFilled(reinterpret_cast<const ImVec2&>(projected_point), radius, colour);
        if (io.MouseClicked[0]) {
            active_handle = pointer_id;
        }
    } else {
        dl->AddCircle(reinterpret_cast<const ImVec2&>(projected_point), radius, colour);
    }
    if (pointer_id == active_handle) {
        math::Mat4x4<float> m;
        m.set_position(math::Vec3<float>(point));
        if (Manipulate(
                current_view_matrix.data(), current_projection_matrix.data(),
                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, m.data())) {
            point = math::Vec3<double>(m.get_position());
            return true;
        }
    }
    return false;
}
