#ifndef GEAROEMIX_RENDER_FONT_2D_HPP
#define GEAROEMIX_RENDER_FONT_2D_HPP
#include "../../math/math-vector.hpp"
#include "rnd-fnt-font.hpp"
namespace gearoenix {
namespace render {
    namespace texture {
        class Texture2D;
    }
    namespace font {
        class Font2D : public Font {
        private:
            static const unsigned int starting_char, ending_char, chars_count;
            struct LetterProperties {
                math::Vec2 uv_min, uv_max;
                math::Vec2 pos_min, pos_max;
            };
            std::vector<LetterProperties> letters_properties;
            std::shared_ptr<texture::Texture2D> baked_texture;

        public:
            Font2D(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Font2D();
        };
    }
}
}
#endif