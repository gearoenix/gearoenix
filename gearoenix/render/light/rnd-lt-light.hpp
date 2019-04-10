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
			math::Vec4 color = math::Vec4(1.0f, 1.0f, 1.0f, 0.7f);

            Light(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Light();

			const math::Vec4& get_color() const;

			bool is_shadower() const;
			void enable_shadowing();
			void disable_shadowing();

			bool is_enabled() const;
			void enable();
			void disable();

			virtual void update_uniform();

			/// Only a shadow caster should have implement this (or in a very rare conditions)
			/// Otherwise the default implementation will return nullptr
			virtual const std::shared_ptr<buffer::FramedUniform> &get_uniform_buffers() const;
        };
    }
}
}
#endif
