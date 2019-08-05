#ifndef GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROENIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-aabb.hpp"
#include "../../math/math-vector.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-lt-type.hpp"
#include <memory>
namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            const Type light_type;
            engine::Engine* const e;
            bool enabled = true;
            bool has_shadow = false;
            math::Vec3 color = math::Vec3(1.0f, 1.0f, 1.0f);
            math::Aabb3 influence;
            buffer::FramedUniform *uniform_buffers = nullptr;

            Light(core::Id my_id, system::stream::Stream* f, engine::Engine* e, Type light_type) noexcept;
            Light(core::Id my_id, engine::Engine* e, Type light_type) noexcept;

        public:
            virtual ~Light() noexcept;

            Type get_type() const noexcept;

            const math::Vec3& get_color() const noexcept;
            void set_color(const math::Vec3& color) noexcept;

            bool is_shadower() const noexcept;
            void enable_shadowing() noexcept;
            void disable_shadowing() noexcept;

            bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;

            const math::Aabb3& get_influence_area() const noexcept;
            void set_influence_area(const math::Aabb3& a) noexcept;

            virtual void update_uniform() noexcept;

            /// Only a shadow caster should have implement this (or in a very rare conditions)
            /// Otherwise the default implementation will return nullptr
            virtual const buffer::FramedUniform* get_uniform_buffers() const noexcept;
            virtual buffer::FramedUniform* get_uniform_buffers() noexcept;
        };
    }
}
}
#endif
