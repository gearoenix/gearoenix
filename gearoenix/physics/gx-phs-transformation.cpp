#include "gx-phs-transformation.hpp"
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/gx-cr-string.hpp"
#include <imgui/imgui.h>

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::TransformationComponent, 8192>::construct();
}

gearoenix::physics::Transformation::Transformation(const Transformation* const parent)
    : x_axis(1.0, 0.0, 0.0)
    , y_axis(0.0, 1.0, 0.0)
    , z_axis(0.0, 0.0, 1.0)
    , scale(1.0)
    , parent(parent)
{
}

void gearoenix::physics::Transformation::set_local_matrix(const math::Mat4x4<double>& lm)
{
    changed = true;
    local_matrix = lm;
    lm.get_axes(x_axis, y_axis, z_axis);
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_global_location() const
{
    return global_matrix.get_location();
}

void gearoenix::physics::Transformation::get_global_location(math::Vec3<double>& l) const
{
    global_matrix.get_location(l);
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_local_location() const
{
    return local_matrix.get_location();
}

void gearoenix::physics::Transformation::get_local_location(math::Vec3<double>& l) const
{
    local_matrix.get_location(l);
}

void gearoenix::physics::Transformation::set_local_location(const math::Vec3<double>& l)
{
    changed = true;
    local_matrix.set_location(l);
}

void gearoenix::physics::Transformation::local_translate(const math::Vec3<double>& t)
{
    changed = true;
    local_matrix.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const double v)
{
    changed = true;
    local_translate(x_axis * v);
}

void gearoenix::physics::Transformation::local_y_translate(const double v)
{
    changed = true;
    local_translate(y_axis * v);
}

void gearoenix::physics::Transformation::local_z_translate(const double v)
{
    changed = true;
    local_translate(z_axis * v);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double d, const math::Vec3<double>& axis, const math::Vec3<double>& l)
{
    changed = true;
    const auto loc = get_local_location();
    local_translate(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_translate((r * math::Vec4(l - loc, 0.0)).xyz() + loc);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double d, const math::Vec3<double>& axis)
{
    changed = true;
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
}

void gearoenix::physics::Transformation::local_rotate(const double d, const math::Vec3<double>& axis)
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_rotate(const math::Quat<double>& q)
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = q.to_mat();
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_x_rotate(const double d)
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(x_axis, d);
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_y_rotate(const double d)
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(y_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::local_z_rotate(const double d)
{
    changed = true;
    const auto loc = local_matrix.get_location();
    local_matrix.set_location(math::Origin3D<double>);
    const auto r = decltype(local_matrix)::rotation(z_axis, d);
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_matrix.set_location(loc);
}

void gearoenix::physics::Transformation::set_local_scale(const math::Vec3<double>& s)
{
    changed = true;
    local_matrix.local_scale(s / scale);
    scale = s;
}

void gearoenix::physics::Transformation::local_scale(const double s)
{
    changed = true;
    local_matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_scale(const math::Vec3<double>& s)
{
    changed = true;
    local_matrix.local_scale(s);
    scale *= s;
}

void gearoenix::physics::Transformation::local_x_scale(const double s)
{
    changed = true;
    local_matrix.local_x_scale(s);
    scale.x *= s;
}

void gearoenix::physics::Transformation::local_y_scale(const double s)
{
    changed = true;
    local_matrix.local_y_scale(s);
    scale.y *= s;
}

void gearoenix::physics::Transformation::local_z_scale(const double s)
{
    changed = true;
    local_matrix.local_z_scale(s);
    scale.z *= s;
}

void gearoenix::physics::Transformation::set_local_orientation(const math::Quat<double>& q)
{
    changed = true;
    const auto r = q.to_mat();
    const auto l = get_local_location();
    local_translate(math::Vec3(0.0));
    x_axis = (r * math::Vec4(x_axis, 0.0)).xyz();
    y_axis = (r * math::Vec4(y_axis, 0.0)).xyz();
    z_axis = (r * math::Vec4(z_axis, 0.0)).xyz();
    local_matrix = r * local_matrix;
    local_translate(l);
}

gearoenix::math::Quat<double> gearoenix::physics::Transformation::get_local_orientation() const
{
    const double trace = x_axis.x + y_axis.y + z_axis.z;
    if (trace > 0.0) {
        const double s = 0.5 / sqrt(trace + 1.0);
        return {
            (y_axis.z - z_axis.y) * s,
            (z_axis.x - x_axis.z) * s,
            (x_axis.y - y_axis.x) * s,
            0.25 / s
        };
    }
    if (x_axis.x > y_axis.y && x_axis.x > z_axis.z) {
        const double s = 0.5 / sqrt(1.0 + x_axis.x - y_axis.y - z_axis.z);
        return {
            0.25 / s,
            (y_axis.x + x_axis.y) * s,
            (z_axis.x + x_axis.z) * s,
            (y_axis.z - z_axis.y) * s,
        };
    }
    if (y_axis.y > z_axis.z) {
        const double s = 0.5 / sqrt(1.0 + y_axis.y - x_axis.x - z_axis.z);
        return {
            (y_axis.x + x_axis.y) * s,
            0.25 / s,
            (z_axis.y + y_axis.z) * s,
            (z_axis.x - x_axis.z) * s
        };
    }
    const double s = 0.5 / sqrt(1.0 + z_axis.z - x_axis.x - y_axis.y);
    return {
        (z_axis.x + x_axis.z) * s,
        (z_axis.y + y_axis.z) * s,
        0.25 / s,
        (x_axis.y - y_axis.x) * s
    };
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& location, const math::Vec3<double>& target, const math::Vec3<double>& up)
{
    changed = true;
    local_matrix = math::Mat4x4<double>::look_at(location, target, up).inverted();
    local_matrix.get_axes(x_axis, y_axis, z_axis);
    scale = math::Vec3(1.0);
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up)
{
    changed = true;
    math::Vec3<double> l;
    get_local_location(l);
    local_look_at(l, target, up);
}

void gearoenix::physics::Transformation::update_without_inverse_root()
{
    if (nullptr != parent) {
        return;
    }
    if (changed) {
        global_matrix = local_matrix;
    }
    for (const auto& child : children) {
        child->update_without_inverse_child();
    }
}

void gearoenix::physics::Transformation::update_inverse()
{
    if (changed) {
        inverted_global_matrix = global_matrix.inverted();
    }
}

void gearoenix::physics::Transformation::update_without_inverse_child()
{
    if (parent->changed) {
        changed = true;
    }
    if (changed) {
        global_matrix = parent->global_matrix * local_matrix;
    }
    for (const auto& child : children) {
        child->update_without_inverse_child();
    }
}

void gearoenix::physics::Transformation::clear_change()
{
    changed = false;
}

void gearoenix::physics::Transformation::reset()
{
    global_matrix = local_matrix = inverted_global_matrix = math::Mat4x4<double>();
    x_axis = math::X3D<double>;
    y_axis = math::Y3D<double>;
    z_axis = math::Z3D<double>;
    scale = math::Vec3(1.0);
    changed = false;
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Quat<double>& r,
    const math::Vec3<double>& l)
{
    local_matrix = r.to_mat();
    x_axis = { local_matrix.data[0][0], local_matrix.data[0][1], local_matrix.data[0][2] };
    y_axis = { local_matrix.data[1][0], local_matrix.data[1][1], local_matrix.data[1][2] };
    z_axis = { local_matrix.data[2][0], local_matrix.data[2][1], local_matrix.data[2][2] };
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_location(l);
    changed = true;
}

void gearoenix::physics::Transformation::add_child(const std::shared_ptr<Transformation>& child)
{
    if (nullptr == child) {
        return;
    }
    children.insert(child);
}

void gearoenix::physics::Transformation::set_parent(const Transformation* const p)
{
    this->parent = p;
}

void gearoenix::physics::Transformation::show_debug_gui_transform(const core::ecs::World& w)
{
    if (!ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        return;
    }

    auto l = get_local_location();
    auto r = get_local_orientation().to_euler() * 180.0 / GX_PI;
    auto s = get_scale();
    bool input_changed = false;

    if (!ImGui::BeginTable("##gearoenix::physics::Transformation", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        return;
    }
    ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.999f);

    ImGui::TableNextColumn();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Position:");
    ImGui::TableNextColumn();
    ImGui::Text("X:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##position.x", &l.x, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##position.y", &l.y, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Z:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##position.z", &l.z, 0.01, 1.0, "%.3f");
    ImGui::TableNextColumn();

    ImGui::Text("Rotation:  ");
    ImGui::TableNextColumn();
    ImGui::Text("X:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##rotation.x", &r.x, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##rotation.y", &r.y, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Z:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##rotation.z", &r.z, 0.01, 1.0, "%.3f");
    ImGui::TableNextColumn();

    ImGui::Text("Scale:");
    ImGui::TableNextColumn();
    ImGui::Text("X:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##scale.x", &s.x, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##scale.y", &s.y, 0.01, 1.0, "%.3f");
    ImGui::SameLine();
    ImGui::Text("Z:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    input_changed |= ImGui::InputDouble("##scale.z", &s.z, 0.01, 1.0, "%.3f");
    ImGui::TableNextColumn();

    ImGui::EndTable();

    if (input_changed) {
        reset(s, math::Quat<double>::from_euler(r * GX_PI / 180.0), l);
    }

    if (ImGui::TreeNode("Children")) {
        for (const auto& child : children) {
            child->show_debug_gui_transform(w);
        }
        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    const math::Vec3<double>& l)
{
    x_axis = x;
    y_axis = y;
    z_axis = z;
    local_matrix.data[0][0] = x.x;
    local_matrix.data[0][1] = x.y;
    local_matrix.data[0][2] = x.z;
    local_matrix.data[0][3] = 0.0;
    local_matrix.data[1][0] = y.x;
    local_matrix.data[1][1] = y.y;
    local_matrix.data[1][2] = y.z;
    local_matrix.data[1][3] = 0.0;
    local_matrix.data[2][0] = z.x;
    local_matrix.data[2][1] = z.y;
    local_matrix.data[2][2] = z.z;
    local_matrix.data[2][3] = 0.0;
    local_matrix.data[3][0] = 0.0;
    local_matrix.data[3][1] = 0.0;
    local_matrix.data[3][2] = 0.0;
    local_matrix.data[3][3] = 1.0;
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_location(l);
    changed = true;
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::physics::TransformationComponent::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types(this);
    return types;
}

gearoenix::physics::TransformationComponent::TransformationComponent(
    std::string&& name, const TransformationComponent* const parent, const core::ecs::entity_id_t entity_id)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , Transformation(parent)
{
}

std::shared_ptr<gearoenix::physics::TransformationComponent> gearoenix::physics::TransformationComponent::construct(
    std::string&& name, TransformationComponent* const parent, const core::ecs::entity_id_t entity_id)
{
    auto self = allocator->make_shared(std::move(name), parent, entity_id);
    self->set_component_self(self);
    if (nullptr != parent) {
        parent->add_child(self);
    }
    return self;
}

void gearoenix::physics::TransformationComponent::show_debug_gui(const core::ecs::World& w)
{
    if (ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        Component::show_debug_gui(w);
        Transformation::show_debug_gui_transform(w);
        ImGui::TreePop();
    }
}

void gearoenix::physics::TransformationComponent::show_debug_gui_transform(const core::ecs::World& w)
{
    show_debug_gui(w);
}

void gearoenix::physics::TransformationComponent::update(core::ecs::World* const world)
{
    world->parallel_system<core::ecs::All<TransformationComponent>>([&](const auto, TransformationComponent* const transform, const auto) {
        transform->update_without_inverse_root();
    });
    world->parallel_system<core::ecs::All<TransformationComponent>>([&](const auto, TransformationComponent* const transform, const auto) {
        transform->update_inverse();
    });
}
