#include "gx-phs-cns-constraint.hpp"
#include "../../render/model/gx-rnd-mdl-model.hpp"

gearoenix::physics::constraint::Constraint::Constraint(const core::Id id, const Type t, std::string n, const bool a) noexcept
    : core::asset::Asset(id, core::asset::Type::Constraint, std::move(n))
    , constraint_type(t)
    , active(a)
{
}

gearoenix::physics::constraint::Constraint::~Constraint() = default;

void gearoenix::physics::constraint::Constraint::add_affected(const std::shared_ptr<render::model::Model>& m) noexcept
{
#ifdef GX_DEBUG_MODE
    if (m->get_model_type() == render::model::Type::Static || affected_models.find(m->get_id()) != affected_models.end())
        GX_UNEXPECTED
#endif
    affected_models.emplace(m->get_id(), m);
}