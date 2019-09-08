#include "rnd-mdl-transformation.hpp"
#include "../../math/math-sphere.hpp"
#include "rnd-mdl-uniform.hpp"
#include "rnd-mdl-model.hpp"

gearoenix::render::model::Transformation::Transformation(Uniform* const uniform, math::Sphere* const sphere, Model * const parent) noexcept
    : uniform(uniform)
    , sphere(sphere)
    , parent(parent)
{
}

void gearoenix::render::model::Transformation::set_location(const math::Vec3& l) noexcept
{
    uniform->m.set_location(l);
    sphere->get_position() = l;
    GXTODO // children, collider
}

void gearoenix::render::model::Transformation::local_scale(const core::Real s) noexcept
{
    uniform->m.local_scale(s);
	sphere->set_radius(sphere->get_radius() * s);
    auto& children = parent->get_children();
    for (auto& c : children) {
        Model* const child = c.second.get();
        auto* const ct = child->get_transformation();
        ct->local_scale(s);
        const auto cl = (child->get_occlusion_sphere().get_position() - sphere->get_position()) * s + sphere->get_position();
        ct->set_location(cl);
    }
    GXTODO // children, collider
}

void gearoenix::render::model::Transformation::local_x_scale(const core::Real s) noexcept
{
    uniform->m.local_scale(s, 1.0f, 1.0f);
	sphere->set_radius(sphere->get_radius() * s);
    GXTODO // children, collider
}
