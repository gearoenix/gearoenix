#ifndef GEAROENIX_RENDER_FONT_MANAGER_HPP
#define GEAROENIX_RENDER_FONT_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"

namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace font {
        class Font;
        class Manager {
        protected:
            engine::Engine* const e;
            core::cache::File<Font> cache;

        public:
            Manager(system::stream::Stream* s, engine::Engine* e) noexcept;
            ~Manager() noexcept = default;
            std::shared_ptr<Font> get(core::Id mid, core::sync::EndCaller<Font> c) noexcept;
        };
    }
}
}
#endif
