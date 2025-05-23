#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-gltf-context.hpp"
#include <unordered_set>

namespace {
#if GX_DEBUG_MODE
std::mutex gx_textures_names_lock;
std::unordered_set<std::string> gx_textures_names;
#endif

auto convert_wrap(const int w)
{
    switch (w) {
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
        return gearoenix::render::texture::Wrap::ClampToEdge;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
        return gearoenix::render::texture::Wrap::Mirror;
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
        return gearoenix::render::texture::Wrap::Repeat;
    default:
        GX_UNEXPECTED;
    }
}
}

gearoenix::render::gltf::Textures::Textures(const Context& c)
    : context(c)
{
}

gearoenix::render::gltf::Textures::~Textures() = default;

void gearoenix::render::gltf::Textures::load(const int index, core::job::EndCaller<>&& end)
{
    const auto& txt = context.data.textures[index];
    GX_LOG_D("Loading texture: " << txt.name);
    const auto& img = context.data.images[txt.source];
    GX_LOG_D("Loading image: " << img.name);
    GX_LOG_D("Loading URI: " << img.uri);
    auto txt_name = img.name;
    if (txt_name.empty()) {
        txt_name = img.uri;
    }
    if (txt_name.empty()) {
        txt_name = txt.name;
    }

#if GX_DEBUG_MODE
    {
        const std::lock_guard _lg(gx_textures_names_lock);
        const auto sz_before = gx_textures_names.size();
        gx_textures_names.insert(txt_name);
        const auto sz_after = gx_textures_names.size();
        GX_ASSERT_D(sz_before < sz_after);
    }
#endif

    const auto& smp = context.data.samplers[txt.sampler];
    GX_LOG_D("Loading sampler: " << smp.name);
    bool needs_mipmap = false;

    const auto convert_filter = [&](const int f) {
        switch (f) {
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
            return texture::Filter::Linear;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
            needs_mipmap = true;
            return texture::Filter::LinearMipmapLinear;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            needs_mipmap = true;
            return texture::Filter::LinearMipmapNearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
            return texture::Filter::Nearest;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            needs_mipmap = true;
            return texture::Filter::NearestMipmapLinear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
            needs_mipmap = true;
            return texture::Filter::NearestMipmapNearest;
        default:
            GX_UNEXPECTED;
        }
    };

    const auto min_filter = convert_filter(smp.minFilter);
    const auto mag_filter = convert_filter(smp.magFilter);

    const auto txt_info = texture::TextureInfo()
                              .set_format(texture::TextureFormat::RgbaUint8)
                              .set_sampler_info(texture::SamplerInfo()
                                      .set_min_filter(min_filter)
                                      .set_mag_filter(mag_filter)
                                      .set_wrap_s(convert_wrap(smp.wrapS))
                                      .set_wrap_t(convert_wrap(smp.wrapT)))
                              .set_width(img.width)
                              .set_height(img.height)
                              .set_depth(0)
                              .set_type(texture::Type::Texture2D)
                              .set_has_mipmap(needs_mipmap);

    core::job::EndCallerShared<texture::Texture2D> txt_end([this, index, end = std::move(end)](std::shared_ptr<texture::Texture2D>&& t) {
        texture_2ds[index] = std::move(t);
        (void)end;
    });

    GX_ASSERT_D(img.width > 0);
    GX_ASSERT_D(img.height > 0);
    GX_ASSERT_D(!img.as_is);
    GX_ASSERT_D(img.image.size() == img.width * img.height * 4);

    texture::Manager::get().create_2d_from_pixels(std::move(txt_name), { img.image }, txt_info, std::move(txt_end));
}

void gearoenix::render::gltf::Textures::load(core::job::EndCaller<>&& end)
{
    const core::job::EndCaller e([e = std::move(end)] {
#if GX_DEBUG_MODE
        gx_textures_names = decltype(gx_textures_names) {};
#endif
        (void)e;
    });

    GX_ASSERT_D(texture_2ds.empty());
    texture_2ds.resize(context.data.textures.size());
    for (int index = 0; index < context.data.textures.size(); ++index) {
        core::job::send_job_to_pool([index, this, e = e]() mutable {
            load(index, std::move(e));
        });
    }
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::gltf::Textures::get(
    const int index,
    const std::shared_ptr<texture::Texture2D>& default_value) const
{
    if (index < 0) {
        return default_value;
    }
    return texture_2ds[index];
}
