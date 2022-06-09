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

    void read(platform::stream::Stream& s) noexcept;
    void write(platform::stream::Stream& s) const noexcept;
};
}
#endif
