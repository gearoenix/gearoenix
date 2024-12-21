#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-gltf-context.hpp"
#include <unordered_set>

namespace {
#if GX_DEBUG_MODE
std::unordered_set<std::string> gx_textures_names;
#endif
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
#if GX_DEBUG_MODE
    {
        const auto sz_before = gx_textures_names.size();
        gx_textures_names.insert(img.name);
        const auto sz_after = gx_textures_names.size();
        GX_ASSERT_D(sz_before < sz_after);
    }
#endif
    GX_ASSERT(!img.as_is); // Only image through bufferView is supported.
    GX_ASSERT(img.bufferView >= 0); // Only image through bufferView is supported.
    GX_ASSERT(img.mimeType.ends_with("jpeg") || img.mimeType.ends_with("png")); // Only these formats are supported.
    const auto& img_bv = context.data.bufferViews[img.bufferView];
    const auto& img_b = context.data.buffers[img_bv.buffer];
    const void* const img_ptr = &img_b.data[img_bv.byteOffset];
    const auto img_sz = static_cast<std::uint32_t>(img_bv.byteLength);
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
    const auto convert_wrap = [&](const int w) {
        switch (w) {
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return texture::Wrap::ClampToEdge;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return texture::Wrap::Mirror;
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return texture::Wrap::Repeat;
        default:
            GX_UNEXPECTED;
        }
    };
    engine::Engine::get()->get_texture_manager()->create_2d_from_formatted(
        std::string(img.name), img_ptr, img_sz,
        texture::TextureInfo()
            .set_format(texture::TextureFormat::Unknown)
            .set_sampler_info(texture::SamplerInfo()
                    .set_min_filter(convert_filter(smp.minFilter))
                    .set_mag_filter(convert_filter(smp.magFilter))
                    .set_wrap_s(convert_wrap(smp.wrapS))
                    .set_wrap_t(convert_wrap(smp.wrapT)))
            .set_width(0)
            .set_height(0)
            .set_depth(0)
            .set_type(texture::Type::Unknown)
            .set_has_mipmap(needs_mipmap),
        core::job::EndCallerShared<texture::Texture2D>(
            [this, index, end = std::move(end)](std::shared_ptr<texture::Texture2D>&& t) {
                texture_2ds[index] = std::move(t);
                (void)end;
            }));
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
