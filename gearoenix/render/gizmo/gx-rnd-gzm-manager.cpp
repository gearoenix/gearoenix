#include "gx-rnd-gzm-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"

#include <boost/functional/hash.hpp>

#include <ImGui/imgui.h>

// ImGuizmo must come after <imgui.h> — it picks up ImGui types from the include above.
#include <ImGuizmo/ImGuizmo.h>

#include <array>
#include <cmath>
#include <tuple>

// =====================================================================================
//  Anonymous helpers for the Blender-style axis-navigation widget.
// =====================================================================================
namespace {

/// Blender-palette axis accents, softened slightly so they sit well on a dark theme.
constexpr std::array<ImU32, 3> k_axis_rgb {
    IM_COL32(232, 71, 86, 255), // X
    IM_COL32(140, 198, 52, 255), // Y
    IM_COL32(65, 146, 240, 255), // Z
};

/// Labels indexed the same way the `axes[1..6]` array is indexed: +X, -X, +Y, -Y, +Z, -Z.
constexpr std::array<const char*, 6> k_axis_labels { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };

/// Replace the alpha byte of an axis colour, keeping the RGB. Used to dim back-facing
/// axis lines and to modulate ring alpha.
[[nodiscard]] constexpr ImU32 axis_color_with_alpha(const int axis_idx, const unsigned int alpha)
{
    return (k_axis_rgb[axis_idx] & 0x00FFFFFFu) | ((alpha & 0xFFu) << IM_COL32_A_SHIFT);
}

/// Draw the line from the gizmo centre to an axis endpoint. For back-facing axes the
/// line is trimmed to stop at the ring's outer edge so it doesn't cross through the
/// empty ring interior; front-facing lines run all the way to the sphere centre
/// (hidden under the filled cap drawn later).
void draw_axis_line(
    ImDrawList* const dl,
    const ImVec2& center, const ImVec2& endpoint,
    const int axis_idx, const bool is_front,
    const float radius, const float line_thickness)
{
    const float dx = endpoint.x - center.x;
    const float dy = endpoint.y - center.y;
    const float len = std::sqrt(dx * dx + dy * dy);
    const float trim = (is_front || len <= radius) ? 0.0f : radius;
    const float t = len > 0.0f ? (len - trim) / len : 1.0f;
    const ImVec2 line_end { center.x + dx * t, center.y + dy * t };
    const ImU32 col = axis_color_with_alpha(axis_idx, is_front ? 235u : 110u);
    dl->AddLine(center, line_end, col, line_thickness);
}

/// Draw a front-facing (camera-ward) axis cap: soft drop shadow, coloured sphere,
/// upper-left gloss highlight, thin dark outline, and the +X/-Y/… label.
void draw_front_cap(
    ImDrawList* const dl,
    const ImVec2& endpoint, const int axis_idx, const int axis_label_slot,
    const float radius, const float shadow_offset,
    const float label_font_size, const int circle_segments)
{
    dl->AddCircleFilled(
        { endpoint.x + shadow_offset, endpoint.y + shadow_offset },
        radius, IM_COL32(0, 0, 0, 90), circle_segments);

    dl->AddCircleFilled(endpoint, radius, k_axis_rgb[axis_idx], circle_segments);

    dl->AddCircleFilled(
        { endpoint.x - radius * 0.32f, endpoint.y - radius * 0.38f },
        radius * 0.30f, IM_COL32(255, 255, 255, 110), 16);

    dl->AddCircle(endpoint, radius, IM_COL32(0, 0, 0, 130), circle_segments, 1.0f);

    const char* const label = k_axis_labels[axis_label_slot];
    const ImVec2 text_sz = ImGui::CalcTextSize(label);
    dl->AddText(nullptr, label_font_size,
        { endpoint.x - text_sz.x * 0.5f, endpoint.y - text_sz.y * 0.5f },
        IM_COL32(255, 255, 255, 245), label);
}

/// Draw a back-facing (away-from-camera) axis cap: just a translucent coloured ring,
/// leaving the interior transparent so the 3D scene shows through — matches Blender's
/// back-cap style.
void draw_back_cap(
    ImDrawList* const dl,
    const ImVec2& endpoint, const int axis_idx,
    const float radius, const float line_thickness, const int circle_segments)
{
    dl->AddCircle(endpoint, radius, axis_color_with_alpha(axis_idx, 200), circle_segments, line_thickness);
}
}

// =====================================================================================
//  Lifecycle
// =====================================================================================

gearoenix::render::gizmo::Manager::Manager()
    : Singleton(this)
{
    enable_translation_handle();
    disable_local_transform_mode();
}

gearoenix::render::gizmo::Manager::~Manager() = default;

// =====================================================================================
//  Viewport-rect resolution — central helper so all three call sites agree.
// =====================================================================================

gearoenix::math::Vec4<float> gearoenix::render::gizmo::Manager::resolved_viewport_rect() const
{
    const bool has_rect = viewport_rect.z > 0.0f && viewport_rect.w > 0.0f;
    const auto& io = ImGui::GetIO();
    return {
        has_rect ? viewport_rect.x : 0.0f,
        has_rect ? viewport_rect.y : 0.0f,
        has_rect ? viewport_rect.z : io.DisplaySize.x,
        has_rect ? viewport_rect.w : io.DisplaySize.y,
    };
}

// =====================================================================================
//  Navigation-axis widget (`show_view`)
// =====================================================================================

void gearoenix::render::gizmo::Manager::show_view()
{
    if (nullptr == current_camera) {
        return;
    }
    const auto& trn = *current_camera->get_transform();
    const auto rect = resolved_viewport_rect();

    // ----- Metrics — everything proportional to the UI font size so the widget
    // rescales cleanly with the Window → UI Text Size slider.
    const float ui_font_size = ImGui::GetFontSize();
    const float width = ui_font_size * 4.5f; // overall diameter
    const float radius = ui_font_size * 0.55f; // axis-cap disc radius
    const float line_thickness = ui_font_size * 0.22f;
    // Margins place the widget so no part of an axis cap can poke past the rect edge.
    // Outermost widget pixel from centre is (half_width + radius), so margin >= radius
    // is the safe lower bound.
    const float margin = radius + ui_font_size * 0.50f;
    const float top_margin = radius + ui_font_size * 0.50f;
    const float label_font_size = ui_font_size;
    const float shadow_offset = std::max(1.0f, ui_font_size * 0.08f);

    // ----- Centre of the widget, in the top-right corner of the 3D rect.
    const float half_width = width * 0.5f;
    const math::Vec3 center(
        rect.x + rect.z - (half_width + margin),
        rect.y + (half_width + top_margin),
        0.0f);
    const auto im_center = ImVec2(center.x, center.y);

    // ----- Axis directions in world space → view space → screen space.
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
    const auto view = current_camera->get_view();
    for (auto& axis : axes) {
        axis = (view * math::Vec4(axis, 0.0f)).xyz() * axes_scale;
    }

    // Translate so the common origin is at the widget centre, and flip Y for ImGui.
    axes[0] -= center;
    axes_z_map.clear();
    for (int i = 1; i < static_cast<int>(axes.size()); ++i) {
        axes[i] -= axes[0];
        axes_z_map.emplace(axes[i].z, i);
        axes[i].y = center.y * 2.0f - axes[i].y;
    }

    // ----- Draw back-to-front in a single pass. Of the six axes, the first three by
    // z are behind the origin (back caps) and the last three are in front (glossy
    // spheres with labels).
    auto* const dl = ImGui::GetWindowDrawList();
    constexpr int circle_segments = 32;

    int draw_idx = 0;
    for (const auto [_, i] : axes_z_map) {
        const bool is_front = ++draw_idx > 3;
        const int axis_idx = (i - 1) >> 1;
        const ImVec2 endpoint { axes[i].x, axes[i].y };

        draw_axis_line(dl, im_center, endpoint, axis_idx, is_front, radius, line_thickness);

        if (is_front) {
            draw_front_cap(dl, endpoint, axis_idx, i - 1,
                radius, shadow_offset, label_font_size, circle_segments);
        } else {
            draw_back_cap(dl, endpoint, axis_idx, radius, line_thickness, circle_segments);
        }
    }
}

// =====================================================================================
//  Per-camera setup — propagates the viewport rect to ImGuizmo and lets each
//  registered drawer emit its gizmos for the current frame.
// =====================================================================================

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

    const auto rect = resolved_viewport_rect();
    ImGuizmo::SetRect(rect.x, rect.y, rect.z, rect.w);

    for (auto* const d : drawers) {
        if (!d->is_gizmo_visible) {
            continue;
        }
        ImGuizmo::PushID(d);
        d->draw_gizmo();
        ImGuizmo::PopID();
    }
}

// =====================================================================================
//  Single-transform and AABB-edit gizmos
// =====================================================================================

bool gearoenix::render::gizmo::Manager::show_transform(math::Mat4x4<core::fp_t>& inout) const
{
    math::Mat4x4<float> m(inout);
    if (!Manipulate(
            current_view_matrix.data(), current_projection_matrix.data(),
            static_cast<ImGuizmo::OPERATION>(operation_handles),
            static_cast<ImGuizmo::MODE>(transform_mode),
            m.data())) {
        return false;
    }
    inout = math::Mat4x4<core::fp_t>(m);
    return true;
}

bool gearoenix::render::gizmo::Manager::show(math::Aabb3<core::fp_t>& box)
{
    std::array<math::Vec3<core::fp_t>, 8> points;
    box.get_all_corners(points);
    bool result = false;

    // Project each corner to the 3D viewport's screen-space rect.
    std::array<math::Vec2<float>, points.size()> display_points;
    const auto rect = resolved_viewport_rect();
    const auto half_rect = math::Vec2(rect.z, rect.w) * 0.5f;
    const auto mid_screen = math::Vec2(rect.x + half_rect.x, rect.y + half_rect.y);
    const auto draw_scale = math::Vec2(half_rect.x, -half_rect.y);
    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto p = current_view_projection_matrix * math::Vec4(math::Vec3<float>(points[i]), 1.0f);
        const auto pp = p.xyz() / p.w;
        if (pp.z > 1.0 || pp.z < -1.0) {
            return false;
        }
        display_points[i] = (pp.xy() * draw_scale) + mid_screen;
    }

    // Determine which box faces face away from the camera (for dim-drawing their edges).
    const auto box_center = math::Vec3<float>(box.get_center());
    const auto box_radius = math::Vec3<float>(box.get_diameter()) * 0.5f;
    constexpr std::array<math::Vec3<float>, 6> unit_directions {
        { { 0, 0, 1 }, { 0, 0, -1 }, { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 } }
    };
    std::array<bool, 6> face_back { };
    for (std::size_t i = 0; i < unit_directions.size(); ++i) {
        const auto& direction = unit_directions[i];
        const auto dir = (box_center + (direction * box_radius)) - current_camera_position;
        face_back[i] = dir.dot(direction) > 0;
    }

    // Box edges as `(from, to, drawn_dim)` triples. An edge is dim when *both* faces
    // adjacent to it are back-facing.
    const std::array<std::tuple<int, int, bool>, 12> edges { {
        { 0, 1, face_back[0] && face_back[4] },
        { 1, 3, face_back[0] && face_back[3] },
        { 3, 2, face_back[0] && face_back[5] },
        { 2, 0, face_back[0] && face_back[2] },
        { 0, 4, face_back[2] && face_back[4] },
        { 1, 5, face_back[3] && face_back[4] },
        { 2, 6, face_back[2] && face_back[5] },
        { 3, 7, face_back[3] && face_back[5] },
        { 4, 5, face_back[1] && face_back[4] },
        { 5, 7, face_back[1] && face_back[3] },
        { 7, 6, face_back[1] && face_back[5] },
        { 6, 4, face_back[1] && face_back[2] },
    } };
    auto* const dl = ImGui::GetWindowDrawList();
    for (const auto& [fi, si, dim] : edges) {
        const auto& fp = display_points[fi];
        const auto& sp = display_points[si];
        dl->AddLine(
            reinterpret_cast<const ImVec2&>(fp), reinterpret_cast<const ImVec2&>(sp),
            dim ? IM_COL32(200, 200, 100, 128) : IM_COL32(250, 250, 140, 128),
            dim ? 1.0f : 3.5f);
    }

    // Per-corner translate handles. The hash seed combines this function's name with
    // the AABB's address and the corner index so each handle gets a unique id across
    // co-existing boxes.
    auto hash = boost::hash_value(__FUNCTION__);
    boost::hash_combine(hash, boost::hash_value(reinterpret_cast<std::uintptr_t>(&box)));
    for (std::size_t i = 0; i < display_points.size(); ++i) {
        auto point_hash = hash;
        boost::hash_combine(point_hash, boost::hash_value(i));
        if (draw_translate_handle(points[i], display_points[i], point_hash)) {
            result = true;
            box.reset(points[i]);
            box.put(points[math::Aabb3<core::fp_t>::counter_corners_indices[i]]);
        }
    }
    return result;
}

// =====================================================================================
//  Trivial accessors / mutators
// =====================================================================================

bool gearoenix::render::gizmo::Manager::is_processing_inputs() const { return ImGuizmo::IsUsingAny(); }

void gearoenix::render::gizmo::Manager::register_drawer(Drawer* const d) { drawers.emplace(d); }
void gearoenix::render::gizmo::Manager::remove_drawer(Drawer* const d) { drawers.erase(d); }

void gearoenix::render::gizmo::Manager::enable_translation_handle() { operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::TRANSLATE); }
void gearoenix::render::gizmo::Manager::disable_translation_handle() { operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::TRANSLATE); }

void gearoenix::render::gizmo::Manager::enable_rotation_handle() { operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::ROTATE); }
void gearoenix::render::gizmo::Manager::disable_rotation_handle() { operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::ROTATE); }

void gearoenix::render::gizmo::Manager::enable_scale_handle() { operation_handles |= static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::SCALE); }
void gearoenix::render::gizmo::Manager::disable_scale_handle() { operation_handles &= ~static_cast<decltype(operation_handles)>(ImGuizmo::OPERATION::SCALE); }

void gearoenix::render::gizmo::Manager::enable_local_transform_mode() { transform_mode = ImGuizmo::MODE::LOCAL; }
void gearoenix::render::gizmo::Manager::disable_local_transform_mode() { transform_mode = ImGuizmo::MODE::WORLD; }

bool gearoenix::render::gizmo::Manager::draw_translate_handle(
    math::Vec3<core::fp_t>& point,
    const math::Vec2<float>& projected_point,
    const std::uintptr_t pointer_id)
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
            point = math::Vec3<core::fp_t>(m.get_position());
            return true;
        }
    }
    return false;
}
