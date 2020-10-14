#include "gx-rnd-rfl-reflection.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../model/gx-rnd-mdl-model.hpp"

gearoenix::render::reflection::Reflection::Reflection(
    const core::Id id,
    std::string name,
    const Type t,
    render::engine::Engine* const e,
    const math::Vec3<double>& position,
    const double influence_radius) noexcept
    : Asset(id, core::asset::Type::Reflection, std::move(name))
    , reflection_type(t)
    , e(e)
    , collider(new physics::collider::Aabb(position + influence_radius, position - influence_radius))
{
}

gearoenix::render::reflection::Reflection::~Reflection() noexcept = default;

void gearoenix::render::reflection::Reflection::update() noexcept
{
    for (auto* const mdl : affected_dynamic_models) {
        mdl->clear_reflection();
    }
    affected_dynamic_models.clear();
}

void gearoenix::render::reflection::Reflection::check_dynamic_models(const physics::accelerator::Bvh* bvh) noexcept
{
    const std::function<void(physics::collider::Collider* const)> collided = [this](physics::collider::Collider* const cld) noexcept {
        auto* const m = cld->get_parent();
        m->set_colliding_reflection(this);
        affected_dynamic_models.push_back(m);
    };
    bvh->call_on_intersecting(collider.get(), collided);
}
