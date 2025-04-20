#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/texture/gx-rnd-txt-manager.hpp"
#include "../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "../render/texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-gl-types.hpp"

namespace gearoenix::gl {
struct Uploader;

[[nodiscard]] sint convert_internal_format(render::texture::TextureFormat f);
[[nodiscard]] enumerated convert_format(render::texture::TextureFormat f);
[[nodiscard]] enumerated convert_data_format(render::texture::TextureFormat f);
[[nodiscard]] float convert_min(render::texture::Filter f);
[[nodiscard]] float convert_mag(render::texture::Filter f);
[[nodiscard]] sint convert(render::texture::Wrap w);
[[nodiscard]] enumerated convert(render::texture::Face f);

struct Texture2D final : render::texture::Texture2D {
    friend struct TextureManager;

    GX_GET_VAL_PRV(uint, object, static_cast<uint>(-1));

    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;
    void* get_imgui_ptr() const override;

public:
    Texture2D(const render::texture::TextureInfo& info, std::string&& name);
    ~Texture2D() override;
    void bind(enumerated texture_unit) const;
    void generate_mipmaps();
};

struct TextureCube final : render::texture::TextureCube {
    friend struct TextureManager;

    GX_GET_VAL_PRV(uint, object, static_cast<uint>(-1));

    void write(const std::shared_ptr<platform::stream::Stream>& s, const core::job::EndCaller<>& c, bool) const override;

public:
    TextureCube(const render::texture::TextureInfo& info, std::string&& name);
    ~TextureCube() override;
    void bind(enumerated texture_unit) const;
};

struct TextureManager final : render::texture::Manager {
    TextureManager();
    ~TextureManager() override;

private:
    void create_2d_from_pixels_v(
        std::string&& name,
        std::vector<std::vector<std::uint8_t>>&& pixels,
        const render::texture::TextureInfo& info,
        core::job::EndCallerShared<render::texture::Texture2D>&& c) override;
    void create_cube_from_pixels_v(
        std::string&& name,
        std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels,
        const render::texture::TextureInfo& info,
        core::job::EndCallerShared<render::texture::TextureCube>&& c) override;
    void create_target_v(
        std::string&& name,
        std::vector<render::texture::Attachment>&& attachments,
        core::job::EndCallerShared<render::texture::Target>&& c) override;
};
}

#endif