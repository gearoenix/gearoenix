#include "gx-phs-transformation.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/gx-cr-string.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "../render/gizmo/gx-rnd-gzm-manager.hpp"
#include <algorithm>
#include <imgui/imgui.h>

gearoenix::physics::Transformation::Transformation(
    std::string&& name,
    Transformation* const parent,
    const core::ecs::entity_id_t entity_id,
    render::engine::Engine* const e)
    : Component(create_this_type_index(this), std::move(name), entity_id)
    , Drawer(e)
    , scale(1.0)
    , parent(parent)
{
    if (nullptr != parent) {
        parent->children.emplace(this);
    }
}

gearoenix::physics::Transformation::~Transformation()
{
    for (const auto& c : children) {
        if (c && this == c->parent) {
            c->parent = nullptr;
        }
    }
    children.clear();
    if (parent) {
        parent->children.erase(this);
        parent = nullptr;
    }
}

void gearoenix::physics::Transformation::set_local_matrix(const math::Mat4x4<double>& lm)
{
    changed = true;
    local_matrix = lm;
    rotation_matrix = lm.to_m3x3();
    rotation_matrix.normalise(scale);
    rotation = decltype(rotation.quat)::from(rotation_matrix);
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_global_position() const
{
    return global_matrix.get_position();
}

void gearoenix::physics::Transformation::get_global_position(math::Vec3<double>& l) const
{
    global_matrix.get_position(l);
}

void gearoenix::physics::Transformation::set_local_position(const math::Vec3<double>& p)
{
    changed = true;
    local_matrix.set_position(p);
}

gearoenix::math::Vec3<double> gearoenix::physics::Transformation::get_local_position() const
{
    return local_matrix.get_position();
}

void gearoenix::physics::Transformation::get_local_position(math::Vec3<double>& p) const
{
    local_matrix.get_position(p);
}

void gearoenix::physics::Transformation::local_translate(const math::Vec3<double>& t)
{
    changed = true;
    local_matrix.translate(t);
}

void gearoenix::physics::Transformation::local_x_translate(const double v)
{
    local_translate(rotation_matrix.columns[0] * v);
}

void gearoenix::physics::Transformation::local_y_translate(const double v)
{
    local_translate(rotation_matrix.columns[1] * v);
}

void gearoenix::physics::Transformation::local_z_translate(const double v)
{
    local_translate(rotation_matrix.columns[2] * v);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double rad, const math::Vec3<double>& axis, const math::Vec3<double>& l)
{
    const auto r = decltype(rotation.quat)::angle_axis(rad, axis);
    const auto position = r.rotate(get_local_position() - l) + l;
    reset(scale, r * rotation.get_quat(), position);
}

void gearoenix::physics::Transformation::local_outer_rotate(const double rad, const math::Vec3<double>& axis)
{
    const auto r = decltype(rotation.quat)::angle_axis(rad, axis);
    const auto position = r.rotate(get_local_position());
    reset(scale, r * rotation.get_quat(), position);
}

void gearoenix::physics::Transformation::local_inner_rotate(const double rad, const math::Vec3<double>& axis)
{
    local_inner_rotate(decltype(rotation.quat)::angle_axis(rad, axis));
}

void gearoenix::physics::Transformation::local_inner_rotate(const math::Quat<double>& q)
{
    reset(scale, q * rotation.get_quat(), get_local_position());
}

void gearoenix::physics::Transformation::local_inner_x_rotate(const double rad)
{
    local_inner_rotate(rad, get_x_axis());
}

void gearoenix::physics::Transformation::local_inner_y_rotate(const double rad)
{
    local_inner_rotate(rad, get_y_axis());
}

void gearoenix::physics::Transformation::local_inner_z_rotate(const double rad)
{
    local_inner_rotate(rad, get_z_axis());
}

void gearoenix::physics::Transformation::set_local_inner_scale(const math::Vec3<double>& s)
{
    changed = true;
    scale = s;
    local_matrix.set_m3x3(rotation_matrix);
    local_matrix.local_scale(s);
}

void gearoenix::physics::Transformation::local_inner_scale(const double s)
{
    changed = true;
    scale = math::Vec3(s);
    local_matrix.local_scale(s);
}

void gearoenix::physics::Transformation::local_inner_scale(const math::Vec3<double>& s)
{
    changed = true;
    scale *= s;
    local_matrix.local_scale(s);
}

void gearoenix::physics::Transformation::local_inner_x_scale(const double s)
{
    changed = true;
    scale.x *= s;
    local_matrix.local_x_scale(s);
}

void gearoenix::physics::Transformation::local_inner_y_scale(const double s)
{
    changed = true;
    scale.y *= s;
    local_matrix.local_y_scale(s);
}

void gearoenix::physics::Transformation::local_inner_z_scale(const double s)
{
    changed = true;
    scale.z *= s;
    local_matrix.local_z_scale(s);
}

void gearoenix::physics::Transformation::set_rotation(const math::Quat<double>& r)
{
    reset(scale, r, get_local_position());
}

void gearoenix::physics::Transformation::set_rotation(const Rotation& r)
{
    reset(scale, r, get_local_position());
}

const gearoenix::math::Vec3<double>& gearoenix::physics::Transformation::get_x_axis() const
{
    return rotation_matrix[0];
}

const gearoenix::math::Vec3<double>& gearoenix::physics::Transformation::get_y_axis() const
{
    return rotation_matrix[1];
}

const gearoenix::math::Vec3<double>& gearoenix::physics::Transformation::get_z_axis() const
{
    return rotation_matrix[2];
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& pos, const math::Vec3<double>& target, const math::Vec3<double>& up)
{
    changed = true;
    local_matrix = decltype(local_matrix)::look_at_inverted(pos, target, up);
    rotation_matrix = local_matrix.to_m3x3();
    rotation = decltype(rotation.quat)::from(rotation_matrix);
    scale = decltype(scale)(1.0);
}

void gearoenix::physics::Transformation::local_look_at(const math::Vec3<double>& target, const math::Vec3<double>& up)
{
    local_look_at(get_local_position(), target, up);
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
    global_matrix = local_matrix = inverted_global_matrix = decltype(local_matrix) {};
    rotation_matrix = {};
    rotation = Rotation();
    scale = decltype(scale)(1.0);
    changed = false;
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const Rotation& r,
    const math::Vec3<double>& p)
{
    changed = true;
    local_matrix = r.get_quat().to_m4x4();
    rotation_matrix = local_matrix.to_m3x3();
    rotation = r;
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_position(p);
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Quat<double>& r,
    const math::Vec3<double>& p)
{
    changed = true;
    rotation = r;
    local_matrix = rotation.get_quat().to_m4x4();
    rotation_matrix = local_matrix.to_m3x3();
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_position(p);
}

void gearoenix::physics::Transformation::reset(
    const math::Vec3<double>& s,
    const math::Vec3<double>& x,
    const math::Vec3<double>& y,
    const math::Vec3<double>& z,
    const math::Vec3<double>& p)
{
    changed = true;
    rotation_matrix = { x, y, z };
    rotation = decltype(rotation.quat)::from(rotation_matrix);
    local_matrix = math::Mat4x4(rotation_matrix);
    scale = s;
    local_matrix.local_scale(s);
    local_matrix.set_position(p);
}

void gearoenix::physics::Transformation::add_child(Transformation* const child)
{
    if (nullptr == child) {
        return;
    }
    children.insert(child);
    child->parent = this;
}

void gearoenix::physics::Transformation::set_parent(Transformation* const p)
{
    parent = p;
    if (nullptr == parent) {
        return;
    }
    parent->children.insert(this);
}

void gearoenix::physics::Transformation::show_debug_gui(const render::engine::Engine& e)
{
    if (!ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        return;
    }

    Component::show_debug_gui(e);

    is_gizmo_visible = true; // maybe later I make it true based on the object selection

    auto l = get_local_position();
    if (!rotation.euler_in_degree.has_value()) {
        rotation.euler_in_degree = rotation.quat.to_euler_degree();
    }
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
    auto rotation_changed = ImGui::InputDouble("##rotation.x", &rotation.euler_in_degree->x, 0.1, 5.0, "%.3f");
    rotation.euler_in_degree->x = math::Numeric::normalise_degree(rotation.euler_in_degree->x);
    ImGui::SameLine();
    ImGui::Text("Y:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    rotation_changed |= ImGui::InputDouble("##rotation.y", &rotation.euler_in_degree->y, 0.1, 5.0, "%.3f");
    rotation.euler_in_degree->y = math::Numeric::normalise_degree(rotation.euler_in_degree->y);
    ImGui::SameLine();
    ImGui::Text("Z:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    rotation_changed |= ImGui::InputDouble("##rotation.z", &rotation.euler_in_degree->z, 0.1, 5.0, "%.3f");
    rotation.euler_in_degree->z = math::Numeric::normalise_degree(rotation.euler_in_degree->z);
    ImGui::TableNextColumn();

    if (rotation_changed) {
        rotation.quat = decltype(rotation.quat)::from_euler_degree(*rotation.euler_in_degree);
        input_changed = true;
    }

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
        reset(s, rotation, l);
    }

    if (ImGui::TreeNode("Children")) {
        for (const auto& child : children) {
            child->show_debug_gui(e);
        }
        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void gearoenix::physics::Transformation::draw_gizmo()
{
    if (gizmo_manager->show_transform(local_matrix)) {
        set_local_matrix(local_matrix);
    }
    is_gizmo_visible = false;
}

void gearoenix::physics::Transformation::update(core::ecs::World* const world)
{
    world->parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->update_without_inverse_root();
    });
    world->parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->update_inverse();
    });
}
