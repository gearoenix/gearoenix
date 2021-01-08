#ifndef GEAROENIX_RENDER_FONT_FONT_HPP
#define GEAROENIX_RENDER_FONT_FONT_HPP
#include "../../core/asset/gx-cr-asset.hpp"
#include "../../core/gx-cr-types.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "gx-rnd-fnt-type.hpp"

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::font {
struct Font : public core::asset::Asset {
protected:
    const Type::Id font_type_id;
    Font(core::Id my_id, std::string name, Type::Id t) noexcept;

public:
    ~Font() noexcept override;
};
}
#endif
