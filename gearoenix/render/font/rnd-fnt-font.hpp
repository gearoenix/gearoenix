#ifndef GEAROEMIX_RENDER_FONT_FONT_HPP
#define GEAROEMIX_RENDER_FONT_FONT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace font {
        class Font : public core::asset::Asset {
        public:
            class Shape {
            public:
                typedef enum : core::Id {
                    D2 = 1,
                    D3 = 2,
                } Type;
            };

        protected:
            const Shape::Type font_type;
            Font(Shape::Type t);

        public:
            virtual ~Font();
            static Font* read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
        };
    }
}
}
#endif