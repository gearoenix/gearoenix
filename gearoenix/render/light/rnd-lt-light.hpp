#ifndef GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-lt-type.hpp"

namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace model {
        class Model;
    }
    namespace light {
        class Light : public core::asset::Asset {
            GX_GETSET_VAL_PRT(core::Real, layer, 0.0f)
            GX_GET_CVAL_PRT(Type, light_type)
            GX_GETSET_CREF_PRT(math::Vec3, color)
        protected:
            engine::Engine* const e;
            bool enabled = true;
            bool has_shadow = false;

            Light(core::Id my_id, system::stream::Stream* f, engine::Engine* e, Type light_type) noexcept;
            Light(core::Id my_id, engine::Engine* e, Type light_type) noexcept;

        public:
            ~Light() noexcept override = default;

            [[nodiscard]] bool is_shadow_caster() const noexcept;
            void enable_shadowing() noexcept;
            void disable_shadowing() noexcept;

            [[nodiscard]] bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;

            virtual bool is_in_light(const model::Model*) const noexcept = 0;
        };
    }
}
}
#endif
