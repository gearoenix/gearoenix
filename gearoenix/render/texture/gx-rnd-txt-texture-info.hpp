#ifndef GEAROENIX_RENDER_TEXTURE_INFO_HPP
#define GEAROENIX_RENDER_TEXTURE_INFO_HPP
#include "gx-rnd-txt-format.hpp"
#include "gx-rnd-txt-sampler.hpp"
#include "gx-rnd-txt-type.hpp"
#include <string>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct TextureInfo final {
    GX_GET_VAL_PRV(std::size_t, hash, 0);
    GX_GET_VAL_PRV(TextureFormat, format, TextureFormat::Unknown);
    GX_GET_CREF_PRV(SamplerInfo, sampler_info);
    GX_GET_VAL_PRV(std::uint32_t, width, 0);
    GX_GET_VAL_PRV(std::uint32_t, height, 0);
    GX_GET_VAL_PRV(std::uint32_t, depth, 0);
    GX_GET_VAL_PRV(Type, type, Type::Unknown);
    GX_GET_VAL_PRV(bool, has_mipmap, true);

public:
    explicit TextureInfo(
        TextureFormat format = TextureFormat::Unknown,
        const SamplerInfo& sampler_info = SamplerInfo(),
        std::uint32_t width = 0,
        std::uint32_t height = 0,
        std::uint32_t depth = 0,
        Type type = Type::Unknown,
        bool has_mipmap = true);

    TextureInfo& set_format(TextureFormat v);
    TextureInfo& set_sampler_info(const SamplerInfo& v);
    TextureInfo& set_width(std::uint32_t v);
    TextureInfo& set_height(std::uint32_t v);
    TextureInfo& set_depth(std::uint32_t v);
    TextureInfo& set_type(Type v);
    TextureInfo& set_has_mipmap(bool v);
    [[nodiscard]] bool operator==(const TextureInfo& o) const;
    void read(platform::stream::Stream& s);
    void write(platform::stream::Stream& s) const;
    void update_hash();
};

struct TextureInfoHasher final {
    [[nodiscard]] inline std::size_t operator()(const TextureInfo& i) const { return i.get_hash(); }
};
}

namespace std {
[[nodiscard]] std::string to_string(const gearoenix::render::texture::TextureInfo& info);
}

#endif
