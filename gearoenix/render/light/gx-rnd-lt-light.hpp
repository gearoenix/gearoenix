#ifndef GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../math/gx-math-vector-3d.hpp"
#include "gx-rnd-lt-type.hpp"

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::light {
class Light : public core::asset::Asset {
    GX_GETSET_VAL_PRT(double, layer, 0.0f)
    GX_GET_CVAL_PRT(Type, light_type)
    GX_GETSET_CREF_PRT(math::Vec3<double>, color)
    GX_GETSET_VAL_PRT(bool, enabled, true)
    GX_GET_VAL_PRT(bool, shadow_enabled, false)
    GX_GET_CPTR_PRT(engine::Engine, e)
    GX_GET_PTR_PRT(scene::Scene, parent_scene)
protected:
    void remove_from_shadow_cascaders() const noexcept;
    void add_to_shadow_cascaders() const noexcept;

    Light(core::Id id, std::string name, system::stream::Stream* f, engine::Engine* e, Type light_type) noexcept;
    Light(core::Id id, std::string name, engine::Engine* e, Type light_type) noexcept;

public:
    ~Light() noexcept override = default;

    void enable_shadowing() noexcept;
    void disable_shadowing() noexcept;

    void set_scene(scene::Scene* s) noexcept;

    virtual bool is_in_light(const model::Model*) const noexcept = 0;
};
}
#endif
