#ifndef GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#define GEAROENIX_RENDER_REFLECTION_REFLECTION_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-static.hpp"
#include "rnd-rfl-type.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::reflection {
class Reflection : public core::asset::Asset {
    GX_GET_CVAL_PRT(Type, reflection_type)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GET_CPTR_PRT(engine::Engine, e)
protected:
    Reflection(core::Id id, Type t, engine::Engine* e) noexcept;

public:
    ~Reflection() noexcept override;
};
}
#endif
