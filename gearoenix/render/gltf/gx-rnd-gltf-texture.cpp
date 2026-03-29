#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-gltf-context.hpp"

#include <set>

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

auto convert_filter(const int f, bool& needs_mipmap)
{
    switch (f) {
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
        needs_mipmap = false;
        return gearoenix::render::texture::Filter::Linear;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        needs_mipmap = true;
        return gearoenix::render::texture::Filter::LinearMipmapLinear;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        needs_mipmap = true;
        return gearoenix::render::texture::Filter::LinearMipmapNearest;
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
        needs_mipmap = false;
        return gearoenix::render::texture::Filter::Nearest;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        needs_mipmap = false;
        return gearoenix::render::texture::Filter::NearestMipmapLinear;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        needs_mipmap = true;
        return gearoenix::render::texture::Filter::NearestMipmapNearest;
    default:
        GX_LOG_D("[WARN] Bad glTF texture filter: " << f);
        return gearoenix::render::texture::Filter::Linear;
    }
}

auto build_sampler_and_mipmap(const gearoenix::render::gltf::Context& context, const int tex_index)
{
    auto sampler = gearoenix::render::texture::SamplerInfo();
    bool needs_mipmap = true;
    if (tex_index >= 0) {
        const auto& txt = context.data.textures[tex_index];
        if (txt.sampler >= 0) {
            const auto& smp = context.data.samplers[txt.sampler];
            sampler.set_min_filter(convert_filter(smp.minFilter, needs_mipmap));
            sampler.set_mag_filter(convert_filter(smp.magFilter, needs_mipmap));
            sampler.set_wrap_s(convert_wrap(smp.wrapS));
            sampler.set_wrap_t(convert_wrap(smp.wrapT));
        }
    }
    return std::pair { sampler, needs_mipmap };
}

auto build_texture_info(const gearoenix::render::gltf::Context& context, const int tex_index)
{
    const auto& img = context.data.images[context.data.textures[tex_index].source];
    auto [sampler, needs_mipmap] = build_sampler_and_mipmap(context, tex_index);
    auto info = gearoenix::render::texture::TextureInfo();
    info.set_format(gearoenix::render::texture::TextureFormat::RgbaUint8)
        .set_sampler_info(sampler)
        .set_width(img.width)
        .set_height(img.height)
        .set_type(gearoenix::render::texture::Type::Texture2D)
        .set_has_mipmap(needs_mipmap);
    return info;
}

auto build_texture_name(const gearoenix::render::gltf::Context& context, const int tex_index, const gearoenix::render::texture::TextureInfo& info)
{
    const auto& txt = context.data.textures[tex_index];
    const auto& img = context.data.images[txt.source];
    auto name = img.name;
    if (name.empty()) {
        name = img.uri;
    }
    if (name.empty()) {
        name = txt.name;
    }
    // TODO: The reason for the following string concatenation is that, right now,
    // the texture buffer and the sampler are one object, later in engine we have to make separation between them.
    name += "-info:" + std::to_string(info.get_hash());
    return name;
}
}

gearoenix::render::gltf::Textures::Textures(const Context& c)
    : context(c)
{
}

gearoenix::render::gltf::Textures::~Textures() = default;

void gearoenix::render::gltf::Textures::load(const int index, core::job::EndCaller<>&& end)
{
    const auto& txt_name = textures_names[index];
    const auto& img = context.data.images[context.data.textures[index].source];

    {
        const std::lock_guard _lg(textures_map_lock);
        if (const auto [_, inserted] = textures_map.emplace(txt_name, nullptr); !inserted) {
            return;
        }
    }

    auto txt_info = build_texture_info(context, index);

    core::job::EndCallerShared<texture::Texture2D> txt_end([this, txt_name, end = std::move(end)](std::shared_ptr<texture::Texture2D>&& t) {
        const std::lock_guard _(textures_map_lock);
        textures_map[txt_name] = std::move(t);
        (void)end;
    });

    GX_ASSERT_D(img.width > 0);
    GX_ASSERT_D(img.height > 0);
    GX_ASSERT_D(!img.as_is);
    GX_ASSERT_D(img.image.size() == img.width * img.height * 4);

    texture::Manager::get().create_2d_from_pixels(std::string(txt_name), { img.image }, txt_info, std::move(txt_end));
}

void gearoenix::render::gltf::Textures::load_orm(const int mr_tex_index, const int occ_tex_index, core::job::EndCaller<>&& end)
{
    const auto key = std::pair { mr_tex_index, occ_tex_index };

    // If both reference the same image source, orm_names already points to the regular texture
    const auto& orm_name = orm_names.at(key);
    {
        const std::lock_guard _lg(textures_map_lock);
        if (const auto [_, inserted] = textures_map.emplace(orm_name, nullptr); !inserted) {
            return;
        }
    }

    const auto has_mr = mr_tex_index >= 0;
    const auto has_occ = occ_tex_index >= 0;
    GX_ASSERT_D(has_mr || has_occ);

    const auto* const mr_img = has_mr ? &context.data.images[context.data.textures[mr_tex_index].source] : nullptr;
    const auto* const occ_img = has_occ ? &context.data.images[context.data.textures[occ_tex_index].source] : nullptr;

    const auto& primary_img = mr_img ? *mr_img : *occ_img;
    const auto width = primary_img.width;
    const auto height = primary_img.height;
    const auto pixel_count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    constexpr auto default_occlusion = static_cast<std::uint8_t>(255); // 1.0
    constexpr auto default_roughness = static_cast<std::uint8_t>(128); // ~0.5
    constexpr auto default_metallic = static_cast<std::uint8_t>(128); // ~0.5

    std::vector<std::uint8_t> pixels(pixel_count * 4);
    const auto mr_matches = mr_img && mr_img->width == width && mr_img->height == height;
    const auto occ_matches = occ_img && occ_img->width == width && occ_img->height == height;

    for (std::size_t i = 0; i < pixel_count; ++i) {
        const auto dst = i * 4;
        // R = Occlusion (from occlusionTexture.R per glTF spec)
        pixels[dst + 0] = occ_matches ? occ_img->image[dst + 0] : default_occlusion;
        // G = Roughness (from metallicRoughnessTexture.G per glTF spec)
        pixels[dst + 1] = mr_matches ? mr_img->image[dst + 1] : default_roughness;
        // B = Metallic (from metallicRoughnessTexture.B per glTF spec)
        pixels[dst + 2] = mr_matches ? mr_img->image[dst + 2] : default_metallic;
        pixels[dst + 3] = 0;
    }

    const auto primary_tex_index = has_mr ? mr_tex_index : occ_tex_index;
    auto [sampler, needs_mipmap] = build_sampler_and_mipmap(context, primary_tex_index);

    auto txt_info = texture::TextureInfo();
    txt_info.set_format(texture::TextureFormat::RgbaUint8)
        .set_sampler_info(sampler)
        .set_width(width)
        .set_height(height)
        .set_type(texture::Type::Texture2D)
        .set_has_mipmap(needs_mipmap);

    core::job::EndCallerShared<texture::Texture2D> txt_end([this, orm_name, end = std::move(end)](std::shared_ptr<texture::Texture2D>&& t) {
        const std::lock_guard _(textures_map_lock);
        textures_map[orm_name] = std::move(t);
        (void)end;
    });

    texture::Manager::get().create_2d_from_pixels(std::string(orm_name), { std::move(pixels) }, txt_info, std::move(txt_end));
}

void gearoenix::render::gltf::Textures::load(core::job::EndCaller<>&& end)
{
    GX_ASSERT_D(textures_names.empty());
    GX_ASSERT_D(textures_map.empty());

    // Pre-compute all texture names synchronously (needed by ORM name resolution)
    textures_names.resize(context.data.textures.size());
    for (int index = 0; index < static_cast<int>(context.data.textures.size()); ++index) {
        auto info = build_texture_info(context, index);
        textures_names[index] = build_texture_name(context, index, info);
    }

    // Pre-compute ORM name mappings synchronously
    std::set<std::pair<int, int>> orm_combinations;
    for (const auto& mat : context.data.materials) {
        const auto mr = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
        const auto occ = mat.occlusionTexture.index;
        if (mr < 0 && occ < 0) {
            continue;
        }
        const auto key = std::pair { mr, occ };
        if (orm_names.contains(key)) {
            continue;
        }
        // Same image source, reuse the existing texture (no merging needed)
        if (mr >= 0 && occ >= 0 && context.data.textures[mr].source == context.data.textures[occ].source) {
            orm_names[key] = textures_names[mr];
            continue;
        }
        // Different images or one missing - will create a merged texture
        orm_combinations.insert(key);
        const auto primary_tex = mr >= 0 ? mr : occ;
        const auto info = build_texture_info(context, primary_tex);
        orm_names[key] = std::string("orm-mr:") + std::to_string(mr) + "-occ:" + std::to_string(occ) + "-info:" + std::to_string(info.get_hash());
    }

    // Dispatch individual texture loads as parallel jobs
    for (int index = 0; index < static_cast<int>(context.data.textures.size()); ++index) {
        core::job::send_job_to_pool([index, this, e = end]() mutable {
            load(index, std::move(e));
        });
    }

    // Dispatch ORM texture builds as parallel jobs
    for (const auto& [mr, occ] : orm_combinations) {
        core::job::send_job_to_pool([mr, occ, this, e = end]() mutable {
            load_orm(mr, occ, std::move(e));
        });
    }
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::gltf::Textures::get(const int index, const std::shared_ptr<texture::Texture2D>& default_value) const
{
    if (index < 0) {
        return default_value;
    }
    const auto search = textures_map.find(textures_names[index]);
    GX_ASSERT_D(search != textures_map.end());
    GX_ASSERT_D(search->second != nullptr);
    return search->second;
}

const std::shared_ptr<gearoenix::render::texture::Texture2D>& gearoenix::render::gltf::Textures::get_orm(const int mr_tex_index, const int occ_tex_index, const std::shared_ptr<texture::Texture2D>& default_value) const
{
    if (mr_tex_index < 0 && occ_tex_index < 0) {
        return default_value;
    }
    const auto key = std::pair { mr_tex_index, occ_tex_index };
    const auto search = orm_names.find(key);
    if (orm_names.end() == search) {
        return default_value;
    }
    const auto tex_search = textures_map.find(search->second);
    GX_ASSERT_D(tex_search != textures_map.end());
    GX_ASSERT_D(tex_search->second != nullptr);
    return tex_search->second;
}