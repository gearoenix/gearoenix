#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-gltf-context.hpp"
#include <unordered_set>

namespace {
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

    bool needs_mipmap = true;
    auto sampler = texture::SamplerInfo();

    if (txt.sampler >= 0) {
        const auto& smp = context.data.samplers[txt.sampler];
        GX_LOG_D("Loading sampler: " << smp.name);

        const auto convert_filter = [&](const int f) {
            switch (f) {
            case TINYGLTF_TEXTURE_FILTER_LINEAR:
                needs_mipmap = false;
                return texture::Filter::Linear;
            case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
                needs_mipmap = true;
                return texture::Filter::LinearMipmapLinear;
            case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
                needs_mipmap = true;
                return texture::Filter::LinearMipmapNearest;
            case TINYGLTF_TEXTURE_FILTER_NEAREST:
                needs_mipmap = false;
                return texture::Filter::Nearest;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
                needs_mipmap = false;
                return texture::Filter::NearestMipmapLinear;
            case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
                needs_mipmap = true;
                return texture::Filter::NearestMipmapNearest;
            default:
                GX_UNEXPECTED;
            }
        };

        sampler.set_min_filter(convert_filter(smp.minFilter));
        sampler.set_mag_filter(convert_filter(smp.magFilter));

        sampler.set_wrap_s(convert_wrap(smp.wrapS));
        sampler.set_wrap_t(convert_wrap(smp.wrapT));
    }

    auto txt_info = texture::TextureInfo();
    txt_info
        .set_format(texture::TextureFormat::RgbaUint8)
        .set_sampler_info(sampler)
        .set_width(img.width)
        .set_height(img.height)
        .set_depth(0)
        .set_type(texture::Type::Texture2D)
        .set_has_mipmap(needs_mipmap);

    auto txt_name = img.name;
    if (txt_name.empty()) {
        txt_name = img.uri;
    }
    if (txt_name.empty()) {
        txt_name = txt.name;
    }

    // TODO: The reason for the following string concatenation is that, right now,
    // the texture buffer and the sampler are one object, later in engine we have to make separation between them.
    txt_name += "-info:" + std::to_string(txt_info.get_hash());
    textures_names[index] = txt_name;

    {
        const std::lock_guard _lg(textures_map_lock);
        if (textures_map.contains(txt_name)) {
            return;
        }
        textures_map.emplace(txt_name, nullptr);
    }

    core::job::EndCallerShared<texture::Texture2D> txt_end([this, txt_name, end = std::move(end)](std::shared_ptr<texture::Texture2D>&& t) {
        const std::lock_guard _(textures_map_lock);
        textures_map[txt_name] = std::move(t);
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
    GX_ASSERT_D(textures_names.empty());
    GX_ASSERT_D(textures_map.empty());
    textures_names.resize(context.data.textures.size());
    for (int index = 0; index < context.data.textures.size(); ++index) {
        core::job::send_job_to_pool([index, this, e = end]() mutable {
            load(index, std::move(e));
        });
    }
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::gltf::Textures::get(
    const int index, const std::shared_ptr<texture::Texture2D>& default_value) const
{
    if (index < 0) {
        return default_value;
    }
    const auto search = textures_map.find(textures_names[index]);
    GX_ASSERT_D(search != textures_map.end());
    GX_ASSERT_D(search->second != nullptr);
    return search->second;
}
