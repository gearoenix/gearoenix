#ifndef GEAROENIX_RENDER_MODEL_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_STATIC_HPP
#include "rnd-mdl-model.hpp"

namespace gearoenix::render::model {
class Static : public Model {
public:
    Static(
        core::Id my_id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Static(
        core::Id my_id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    void set_scene(scene::Scene* s) noexcept final;
    bool get_dynamicity() const noexcept final;
};
}
#endif
