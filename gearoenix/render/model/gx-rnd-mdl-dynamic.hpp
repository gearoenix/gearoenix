#ifndef GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#include "gx-rnd-mdl-model.hpp"

namespace gearoenix::render::model {
class Dynamic final : public Model {
    GX_GET_CREF_PRV(std::weak_ptr<Dynamic>, dynamic_model_self)
private:
    Dynamic(
        core::Id id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Dynamic(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    Dynamic(const Dynamic& o) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Dynamic> construct(
        core::Id id,
        std::string name,
        system::stream::Stream* f,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    [[nodiscard]] static std::shared_ptr<Dynamic> construct(
        core::Id id,
        std::string name,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Dynamic() noexcept final;
    [[nodiscard]] bool get_dynamicity() const noexcept final { return true; }
    [[nodiscard]] std::shared_ptr<Model> clone() const noexcept final;
};
}
#endif
