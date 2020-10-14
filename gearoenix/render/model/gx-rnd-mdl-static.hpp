#ifndef GEAROENIX_RENDER_MODEL_STATIC_HPP
#define GEAROENIX_RENDER_MODEL_STATIC_HPP
#include "gx-rnd-mdl-model.hpp"

namespace gearoenix::render::model {
class Static final : public Model {
    GX_GET_CREF_PRV(std::shared_ptr<Static>, static_model_self)
private:
    Static(
        core::Id id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Static(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Static> construct(
        core::Id id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Static> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Static() noexcept final;
    void set_scene(scene::Scene* s) noexcept final;
    [[nodiscard]] bool get_dynamicity() const noexcept final;
};
}
#endif
