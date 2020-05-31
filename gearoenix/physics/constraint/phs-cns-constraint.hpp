#ifndef GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#define GEAROENIX_PHYSICS_CONSTRAINT_CONSTRAINT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../system/sys-log.hpp"
#include "phs-cns-type.hpp"
#include <map>
#include <memory>

namespace gearoenix::core::event {
class Engine;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::physics::constraint {
class Constraint : public core::asset::Asset {
public:
    typedef std::map<core::Id, std::shared_ptr<render::model::Model>> ModelMap;
    typedef std::map<core::Id, std::shared_ptr<Constraint>> ConstraintMap;
    GX_GET_CVAL_PRT(Type, constraint_type)
    GX_GET_CVAL_PRT(bool, active)
    GX_GET_VAL_PRT(bool, enabled, true)
    GX_GET_CREF_PRT(ModelMap, affected_models)
    GX_GET_CREF_PRT(ConstraintMap, after_constraints)

protected:
    Constraint(core::Id id, Type t, bool active = true) noexcept;

    /// Thi function will be called after the parent constraint applied on its models
    virtual void update() noexcept { }

public:
    ~Constraint() override;
    virtual void apply(double delta_time_from_start, double delta_time_from_previous) noexcept = 0;

    void add(const std::shared_ptr<Constraint>& c) noexcept
    {
#ifdef GX_DEBUG_MODE
        if (after_constraints.find(c->get_asset_id()) != after_constraints.end())
            GX_UNEXPECTED
#endif
        after_constraints.emplace(c->get_asset_id(), c);
    }

    void add_affected(const std::shared_ptr<render::model::Model>& m) noexcept;
};
}
#endif
