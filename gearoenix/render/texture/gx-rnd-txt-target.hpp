#pragma once
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-vector-2d.hpp"
#include "gx-rnd-txt-attachment.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Target {
    GX_GET_REFC_PRT(std::string, name);
    GX_GET_CREF_PRT(std::vector<Attachment>, attachments);

    explicit Target(std::string&& name, std::vector<Attachment>&& attachments);

public:
    virtual ~Target();
    [[nodiscard]] math::Vec2<std::uint32_t> get_dimension() const;
    [[nodiscard]] float get_aspect_ratio() const;
    void write(std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const;
};
}