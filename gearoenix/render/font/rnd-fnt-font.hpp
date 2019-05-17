#ifndef GEAROENIX_RENDER_FONT_FONT_HPP
#define GEAROENIX_RENDER_FONT_FONT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-fnt-type.hpp"
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
    namespace font {
        class Font {
        protected:
            const Type::Id font_type_id;
            Font(const core::Id my_id, const Type::Id t);

        public:
            virtual ~Font();
            static Font* read(
                const core::Id font_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& c);
        };
    }
}
}
#endif
