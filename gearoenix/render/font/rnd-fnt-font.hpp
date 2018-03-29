#ifndef GEAROEMIX_RENDER_FONT_FONT_HPP
#define GEAROEMIX_RENDER_FONT_FONT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
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
            Font(core::Id my_id, Shape::Type t);

        public:
            virtual ~Font();
            static Font* read(core::Id font_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
        };
    }
}
}
#endif
