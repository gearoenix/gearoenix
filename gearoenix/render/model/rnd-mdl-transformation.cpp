#include "rnd-mdl-transformation.hpp"
#include "../../math/math-sphere.hpp"
#include "../../physics/phs-transformation.hpp"
#include "rnd-mdl-model.hpp"
#include "rnd-mdl-uniform.hpp"

gearoenix::render::model::Transformation::Transformation(Uniform* const uniform, math::Sphere* const sphere, Model* const parent) noexcept
    : uniform(uniform)
    , sphere(sphere)
    , parent(parent)
{
}

const gearoenix::math::Vec3& gearoenix::render::model::Transformation::get_location() const noexcept
{
    return sphere->get_center();
}

void gearoenix::render::model::Transformation::set_location(const math::Vec3& l) noexcept
{
    const math::Vec3 trans = l - sphere->get_center();
    uniform->m.set_location(l);
    sphere->set_center(l);
    auto* const collider = parent->get_collider();
    if (nullptr != collider)
        collider->set_location(l);
    auto& children = parent->get_children();
    for (auto& c : children)
        c.second->get_transformation()->translate(trans);
}

void gearoenix::render::model::Transformation::translate(const math::Vec3& l) noexcept
{
    set_location(get_location() + l);
}

void gearoenix::render::model::Transformation::local_scale(const core::Real s) noexcept
{
    uniform->m.local_scale(s);
    sphere->set_radius(sphere->get_radius() * s);
    auto* const collider = parent->get_collider();
    if (nullptr != collider)
        collider->local_scale(s);
    auto& children = parent->get_children();
    for (auto& c : children) {
        Model* const child = c.second.get();
        auto* const ct = child->get_transformation();
        ct->local_scale(s);
        const auto cl = (ct->get_location() - sphere->get_center()) * s + sphere->get_center();
        ct->set_location(cl);
    }
}

void gearoenix::render::model::Transformation::local_x_scale(const core::Real s) noexcept
{
	uniform->m.local_scale(s, 1.0f, 1.0f);
	auto* const collider = parent->get_collider();
	if (nullptr != collider)
		collider->local_x_scale(s);
	auto& children = parent->get_children();
	for (auto& c : children) {
		Model* const child = c.second.get();
		auto* const ct = child->get_transformation();
		ct->local_x_scale(s);
		const auto cl = (ct->get_location() - sphere->get_center()) * math::Vec3(s, 1.0f, 1.0f) + sphere->get_center();
		ct->set_location(cl);
	}
}

void gearoenix::render::model::Transformation::local_y_scale(const core::Real s) noexcept
{
	uniform->m.local_scale(1.0f, s, 1.0f);
	auto* const collider = parent->get_collider();
	if (nullptr != collider)
		collider->local_y_scale(s);
	auto& children = parent->get_children();
	for (auto& c : children) {
		Model* const child = c.second.get();
		auto* const ct = child->get_transformation();
		ct->local_y_scale(s);
		const auto cl = (ct->get_location() - sphere->get_center()) * math::Vec3(1.0f, s, 1.0f) + sphere->get_center();
		ct->set_location(cl);
	}
}

void gearoenix::render::model::Transformation::local_z_scale(const core::Real s) noexcept
{
	uniform->m.local_scale(1.0f, 1.0f, s);
	auto* const collider = parent->get_collider();
	if (nullptr != collider)
		collider->local_z_scale(s);
	auto& children = parent->get_children();
	for (auto& c : children) {
		Model* const child = c.second.get();
		auto* const ct = child->get_transformation();
		ct->local_z_scale(s);
		const auto cl = (ct->get_location() - sphere->get_center()) * math::Vec3(1.0f, s, 1.0f) + sphere->get_center();
		ct->set_location(cl);
	}
}
