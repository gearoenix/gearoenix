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
    namespace engine {
        class Engine;
    }
    namespace light {
        class Light : public core::asset::Asset {
        protected:
			bool enabled = true;
			bool has_shadow = false;
			math::Vec4 color_strength = math::Vec4(1.0f, 1.0f, 1.0f, 0.7f);

            Light(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Light();

			const math::Vec3& get_color() const;
			const math::Vec4& get_color_strength() const;
			const core::Real strength() const;

			bool is_shadower() const;
			void enable_shadowing();
			void disable_shadowing();

			bool is_enabled() const;
			void enable();
			void disable();
        };
    }
}
}
#endif
