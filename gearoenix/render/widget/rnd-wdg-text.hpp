#ifndef GEAROEMIX_RENDER_WIDGET_TEXT_HPP
#define GEAROEMIX_RENDER_WIDGET_TEXT_HPP
#include "rnd-wdg-widget.hpp"
#include <cstdint>
namespace gearoenix {
namespace render {
    namespace font {
        class Font;
        class Font2D;
    }
    namespace widget {
        class Text : public Widget {
            friend class physics::Kernel;

        public:
            class Alignment {
            public:
                typedef enum : std::uint8_t {
                    CENTER_BOTTOM = 1,
                    CENTER_MIDDLE = 2,
                    CENTER_TOP = 3,
                    LEFT_BOTTOM = 4,
                    LEFT_MIDDLE = 5,
                    LEFT_TOP = 6,
                    RIGHT_BOTTOM = 7,
                    RIGHT_MIDDLE = 8,
                    RIGHT_TOP = 9,
                } Type;
            };

        private:
            std::string text;
            Alignment::Type align;
            std::shared_ptr<font::Font2D> fnt;
            std::shared_ptr<mesh::Mesh> msh;
            core::Id mesh_id;
            void create_text_mesh(core::EndCaller<core::EndCallerIgnore> c);

        public:
            Text(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Text();
        };
    }
}
}
#endif
