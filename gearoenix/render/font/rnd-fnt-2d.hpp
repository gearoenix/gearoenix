#ifndef GEAROEMIX_RENDER_FONT_2D_HPP
#define GEAROEMIX_RENDER_FONT_2D_HPP
#include "../../math/math-vector.hpp"
#include "rnd-fnt-font.hpp"
#include <vector>
namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
    namespace texture {
        class Texture2D;
    }
    namespace font {
        class Font2D : public Font {
        public:
            struct LetterProperties {
                math::Vec2 uv_min, uv_max;
                math::Vec2 pos_min, pos_max;
            };
            static const int baked_aspect;
            static const int first_character;
            static const int last_character;
            static const int characters_count;

        private:
            std::vector<LetterProperties> letters_properties;
            std::shared_ptr<texture::Texture2D> baked_texture;

        public:
            Font2D(
				const core::Id my_id, 
				const std::shared_ptr<system::stream::Stream> &f, 
				const std::shared_ptr<engine::Engine> &e, 
				const core::sync::EndCaller<core::sync::EndCallerIgnore> &c);
            ~Font2D();
            const LetterProperties& get_letter_properties(char c) const;
            const std::shared_ptr<texture::Texture2D> get_baked_texture() const;
        };
    }
}
}
#endif
