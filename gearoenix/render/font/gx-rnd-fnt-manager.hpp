#ifndef GEAROENIX_RENDER_FONT_MANAGER_HPP
#define GEAROENIX_RENDER_FONT_MANAGER_HPP
#include "../../core/cache/gx-cr-cache-file.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"

namespace gearoenix::ystem::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::font {
struct Font;
struct Font2D;
struct Manager {
    GX_GET_CREF_PRV(core::cache::File<Font>, cache)
protected:
    engine::Engine* const e;
    std::shared_ptr<Font2D> default_2d;

public:
    Manager(std::unique_ptr<platform::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    std::shared_ptr<Font> get(core::Id mid, core::sync::EndCaller<Font> c) noexcept;
    std::shared_ptr<Font2D> get_default_2d(core::sync::EndCaller<Font> c) noexcept;
};
}
#endif
