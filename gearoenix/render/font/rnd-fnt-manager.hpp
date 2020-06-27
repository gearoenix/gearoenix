#ifndef GEAROENIX_RENDER_FONT_MANAGER_HPP
#define GEAROENIX_RENDER_FONT_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"

namespace gearoenix::ystem::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::font {
class Font;
class Font2D;
class Manager {
protected:
    engine::Engine* const e;
    core::cache::File<Font> cache;
    std::shared_ptr<Font2D> default_2d;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    std::shared_ptr<Font> get(core::Id mid, core::sync::EndCaller<Font> c) noexcept;
    std::shared_ptr<Font2D> get_default_2d(core::sync::EndCaller<Font> c) noexcept;
};
}
#endif
