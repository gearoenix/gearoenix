#include "gx-phs-transformation.hpp"
#include "../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "../render/gizmo/gx-rnd-gzm-manager.hpp"
#include "../render/imgui/gx-rnd-imgui-math-vec.hpp"
#include "../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include <algorithm>

void gearoenix::physics::Transformation::write_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<>&&) const
{
    local_matrix.get_position().write(*s);
    rotation.get_quat().write(*s);
    scale.write(*s);

    s->write_fail_debug(static_cast<std::uint32_t>(children.size()));
    for (const auto* const c : children) {
        s->write_fail_debug(c->get_entity_id());
    }
    const auto has_parent = nullptr != parent;
    s->write_fail_debug(has_parent);
    if (has_parent) {
        s->write_fail_debug(parent->get_entity_id());
    }
}

void gearoenix::physics::Transformation::update_in_io_context(
    std::shared_ptr<platform::stream::Stream>&& s,
    core::job::EndCaller<>&& e)
{
    math::Vec3<double> pos;
    pos.read(*s);
    math::Quat<double> rot;
    rot.read(*s);
    math::Vec3<double> scl;
    scl.read(*s);

    core::job::EndCaller end([this, s = component_self.lock(), e = std::move(e), pos, rot, scl] {
        reset(scl, rot, pos);
        (void)s;
        (void)e;
    });

    const auto children_count = s->read<std::uint32_t>();
    children.clear();
    const auto children_lock = std::make_shared<std::mutex>();
    for (auto i = decltype(children_count) { 0 }; i < children_count; ++i) {
        core::ecs::World::get()->resolve([this, children_lock, id = s->read<core::ecs::entity_id_t>(), end] {
            auto* const child = core::ecs::World::get()->get_component<Transformation>(id);
            if (nullptr == child) {
                return true;
            }
            const std::lock_guard _lg(*children_lock);
            children.emplace(child);
            (void)end;
            return false;
        });
    }
    if (s->read<bool>()) {
        core::ecs::World::get()->resolve([this, id = s->read<core::ecs::entity_id_t>(), end = std::move(end)] {
            auto* const p = core::ecs::World::get()->get_component<Transformation>(id);
            if (nullptr == p) {
                return true;
            }
            parent = p;
            (void)end;
            return false;
        });
    }
}

gearoenix::physics::Transformation::Transformation(
    std::string&& name,
    Transformation* const parent,
    const core::ecs::entity_id_t entity_id)
    : Component(core::ecs::ComponentType::create_index(this), std::move(name), entity_id)
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

void gearoenix::physics::Transformation::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Component::show_debug_gui();
        is_gizmo_visible = true; // maybe later I make it true based on the object selection

        auto l = get_local_position();
        if (!rotation.euler_in_degree.has_value()) {
            rotation.euler_in_degree = rotation.quat.to_euler_degree();
        }
        auto s = get_scale();
        bool input_changed = false;

        render::imgui::table_scope("##gearoenix::physics::Transformation", [&, this] {
            ImGui::Text("Position:");
            ImGui::TableNextColumn();
            input_changed |= render::imgui::show(l);
            ImGui::TableNextColumn();

            ImGui::Text("Rotation:  ");
            ImGui::TableNextColumn();
            if (render::imgui::show(*rotation.euler_in_degree)) {
                rotation.euler_in_degree->normalise_degree();
                rotation.quat = decltype(rotation.quat)::from_euler_degree(*rotation.euler_in_degree);
                input_changed = true;
            }
            ImGui::TableNextColumn();

            ImGui::Text("Scale:");
            ImGui::TableNextColumn();
            input_changed |= render::imgui::show(s);
        });

        if (input_changed) {
            reset(s, rotation, l);
        }

        if (ImGui::TreeNode("Children")) {
            for (const auto& child : children) {
                child->show_debug_gui();
            }
            ImGui::TreePop();
        }
    });
}

void gearoenix::physics::Transformation::draw_gizmo()
{
    if (gizmo_manager->show_transform(local_matrix)) {
        set_local_matrix(local_matrix);
    }
    is_gizmo_visible = false;
}

void gearoenix::physics::Transformation::update()
{
    core::ecs::World::get()->parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->update_without_inverse_root();
    });
    core::ecs::World::get()->parallel_system<Transformation>([&](const auto, auto* const transform, const auto) {
        transform->update_inverse();
    });
}
