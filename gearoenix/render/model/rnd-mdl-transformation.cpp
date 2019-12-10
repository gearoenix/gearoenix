#include "rnd-mdl-transformation.hpp"
#include "../../physics/phs-transformation.hpp"
#include "rnd-mdl-model.hpp"

gearoenix::render::model::Transformation::Transformation(Model* const parent) noexcept
    : parent(parent)
{
}

const gearoenix::math::Vec3& gearoenix::render::model::Transformation::get_location() const noexcept
{
    return parent->get_collider()->get_location();
}

void gearoenix::render::model::Transformation::set_location(const math::Vec3& l) noexcept
{
    physics::collider::Collider* const collider = parent->get_collider();
    const math::Vec3 trans = l - collider->get_location();
    collider->set_location(l);
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = parent->get_children();
    for (const std::pair<const core::Id, std::shared_ptr<model::Model>>& c : children)
        c.second->get_transformation()->translate(trans);
}

void gearoenix::render::model::Transformation::translate(const math::Vec3& l) noexcept
{
    set_location(get_location() + l);
}

void gearoenix::render::model::Transformation::local_scale(const core::Real s) noexcept
{
    const math::Vec3& loc = get_location();
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = parent->get_children();
    for (const std::pair<const core::Id, std::shared_ptr<model::Model>>& c : children) {
        Model* const child = c.second.get();
        physics::Transformation* const ct = child->get_transformation();
        ct->local_scale(s);
        ct->set_location((ct->get_location() - loc) * s + loc);
    }
    parent->get_collider()->local_scale(s);
    ;
}

void gearoenix::render::model::Transformation::local_x_scale(const core::Real s) noexcept
{
    const math::Vec3& loc = get_location();
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = parent->get_children();
    for (const std::pair<const core::Id, std::shared_ptr<model::Model>>& c : children) {
        Model* const child = c.second.get();
        physics::Transformation* const ct = child->get_transformation();
        ct->local_x_scale(s);
        math::Vec3 l = ct->get_location() - loc;
        l[0] *= s;
        ct->set_location(l + loc);
    }
    parent->get_collider()->local_x_scale(s);
}

void gearoenix::render::model::Transformation::local_y_scale(const core::Real s) noexcept
{
    const math::Vec3& loc = get_location();
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = parent->get_children();
    for (const std::pair<const core::Id, std::shared_ptr<model::Model>>& c : children) {
        Model* const child = c.second.get();
        physics::Transformation* const ct = child->get_transformation();
        ct->local_y_scale(s);
        math::Vec3 l = ct->get_location() - loc;
        l[1] *= s;
        ct->set_location(l + loc);
    }
    parent->get_collider()->local_y_scale(s);
}

void gearoenix::render::model::Transformation::local_z_scale(const core::Real s) noexcept
{
    const math::Vec3& loc = get_location();
    const std::map<core::Id, std::shared_ptr<model::Model>>& children = parent->get_children();
    for (const std::pair<const core::Id, std::shared_ptr<model::Model>>& c : children) {
        Model* const child = c.second.get();
        physics::Transformation* const ct = child->get_transformation();
        ct->local_z_scale(s);
        math::Vec3 l = ct->get_location() - loc;
        l[2] *= s;
        ct->set_location(l + loc);
    }
    parent->get_collider()->local_z_scale(s);
}
