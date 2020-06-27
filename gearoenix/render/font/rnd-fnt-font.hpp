#ifndef GEAROENIX_RENDER_FONT_FONT_HPP
#define GEAROENIX_RENDER_FONT_FONT_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-fnt-type.hpp"

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::font {
class Font : public core::asset::Asset {
protected:
    const Type::Id font_type_id;
    Font(core::Id my_id, std::string name, Type::Id t) noexcept;

public:
    ~Font() noexcept override;
};
}
#endif
