#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-phs-cns-type.hpp"
#include <map>
#include <memory>

namespace gearoenix::core::event {
struct Engine;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::model {
struct Model;
}

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::physics::constraint {
struct Constraint : public core::asset::Asset {
public:
    typedef std::map<core::Id, std::shared_ptr<render::model::Model>> ModelMap;
    typedef std::map<core::Id, std::shared_ptr<Constraint>> ConstraintMap;
    GX_GET_CVAL_PRT(Type, constraint_type)
    GX_GET_CVAL_PRT(bool, active)
    GX_GET_VAL_PRT(bool, enabled, true)
    GX_GET_CREF_PRT(ModelMap, affected_models)
    GX_GET_CREF_PRT(ConstraintMap, after_constraints)

protected:
    Constraint(core::Id id, Type t, std::string name, bool active = true) noexcept;

    /// This function will be called after the parent constraint applied on its models
    virtual void update() noexcept { }
    void update_chained_constraints() noexcept
    {
        for (const auto& c : after_constraints) {
            c.second->update();
        }
    }

public:
    ~Constraint() override;
    virtual void apply(double delta_time_from_start, double delta_time_from_previous) noexcept = 0;

    void add(const std::shared_ptr<Constraint>& c) noexcept
    {
#ifdef GX_DEBUG_MODE
        if (after_constraints.find(c->get_id()) != after_constraints.end())
            GX_UNEXPECTED
#endif
        after_constraints.emplace(c->get_id(), c);
    }

    void add_affected(const std::shared_ptr<render::model::Model>& m) noexcept;
};
}
#endif
