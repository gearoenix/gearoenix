#ifndef GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#define GEAROEMIX_RENDER_LIGHT_LIGHT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../math/math-vector.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace buffer {
        class FramedUniform;
    }
    namespace engine {
        class Engine;
    }
    namespace light {
        class Light : public core::asset::Asset {
        protected:
            bool enabled = true;
            bool has_shadow = false;
            math::Vec3 color = math::Vec3(1.0f, 1.0f, 1.0f);

			Light(
				const core::Id my_id,
				const std::shared_ptr<system::stream::Stream>& f,
				const std::shared_ptr<engine::Engine>& e) noexcept;
			Light(
				const core::Id my_id,
				const std::shared_ptr<engine::Engine>& e) noexcept;

        public:
            virtual ~Light() noexcept;

			const math::Vec3& get_color() const noexcept;
			void set_color(const math::Vec3& color) noexcept;

            bool is_shadower() const noexcept;
            void enable_shadowing() noexcept;
            void disable_shadowing() noexcept;

            bool is_enabled() const noexcept;
            void enable() noexcept;
            void disable() noexcept;

            virtual void update_uniform() noexcept;

            /// Only a shadow caster should have implement this (or in a very rare conditions)
            /// Otherwise the default implementation will return nullptr
            virtual const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const noexcept;
        };
    }
}
}
#endif
