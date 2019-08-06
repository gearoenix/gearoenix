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
        protected:
            const Type light_type;
            engine::Engine* const e;
            bool enabled = true;
            bool has_shadow = false;
            math::Vec3 color = math::Vec3(1.0f, 1.0f, 1.0f);

            Light(core::Id my_id, system::stream::Stream* f, engine::Engine* e, Type light_type) noexcept;
            Light(core::Id my_id, engine::Engine* e, Type light_type) noexcept;

        public:
            virtual ~Light() noexcept = default;

            Type get_type() const noexcept;

            const math::Vec3& get_color() const noexcept;
            void set_color(const math::Vec3& color) noexcept;

            bool is_shadow_caster() const noexcept;
            void enable_shadowing() noexcept;
            void disable_shadowing() noexcept;

            bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;

            virtual bool is_in_light(const model::Model*) const noexcept = 0;
        };
    }
}
}
#endif
