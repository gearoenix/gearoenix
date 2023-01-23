#ifndef GEAROENIX_RENDER_TEXTURE_INFO_HPP
#define GEAROENIX_RENDER_TEXTURE_INFO_HPP
#include "gx-rnd-txt-format.hpp"
#include "gx-rnd-txt-sampler.hpp"
#include "gx-rnd-txt-type.hpp"

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct TextureInfo final {
    TextureFormat format = TextureFormat::Unknown;
    SamplerInfo sampler_info;
    std::size_t width = 0;
    std::size_t height = 0;
    std::size_t depth = 0;
    Type type = Type::Unknown;
    bool has_mipmap = true;

    [[nodiscard]] bool operator==(const TextureInfo& o) const noexcept;
    void read(platform::stream::Stream& s) noexcept;
    void write(platform::stream::Stream& s) const noexcept;
};

struct TextureInfoHasher final {
    [[nodiscard]] std::size_t operator()(const TextureInfo&) const noexcept;
};
}
#endif
