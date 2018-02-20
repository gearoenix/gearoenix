#ifndef GEAROEMIX_RENDER_FONT_2D_HPP
#define GEAROEMIX_RENDER_FONT_2D_HPP
#include "../../math/math-vector.hpp"
#include "rnd-fnt-font.hpp"
#include <vector>
namespace gearoenix {
namespace render {
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

        private:
            std::vector<LetterProperties> letters_properties;
            std::shared_ptr<texture::Texture2D> baked_texture;

        public:
            Font2D(core::Id my_id, system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Font2D();
            const LetterProperties& get_letter_properties(char c) const;
            const std::shared_ptr<texture::Texture2D> get_baked_texture() const;
        };
    }
}
}
#endif
