#include "gx-rnd-gzm-manager.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include <imgui/imgui.h>

gearoenix::render::gizmo::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::gizmo::Manager::~Manager() = default;

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

    axes[0] = math::Vec3<float>(trn.get_local_location() + (trn.get_z_axis() * z_dis_origin));
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
}
