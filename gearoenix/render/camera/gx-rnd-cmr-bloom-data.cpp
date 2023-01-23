#include "gx-rnd-cmr-bloom-data.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include <format>

gearoenix::render::camera::BloomData::BloomData(
    engine::Engine& e,
    const std::string& name,
    const core::sync::EndCaller& end_caller,
    const std::shared_ptr<texture::Target>& target) noexcept
    : scatter_clamp_max_threshold_threshold_knee(0.8f, 7.0f, 0.9f, 0.5f)
{
    const auto dim = target->get_dimension();
    auto* const txt_mgr = e.get_texture_manager();
    const render::texture::TextureInfo h_txt_info {
        .format = render::texture::TextureFormat::RgbaFloat16,
        .sampler_info = render::texture::SamplerInfo {
            .min_filter = render::texture::Filter::LinearMipmapNearest,
            .mag_filter = render::texture::Filter::Linear,
            .wrap_s = render::texture::Wrap::ClampToEdge,
            .wrap_t = render::texture::Wrap::ClampToEdge,
            .wrap_r = render::texture::Wrap::ClampToEdge,
            .anisotropic_level = 0,
        },
        .width = dim.x,
        .height = dim.y,
        .depth = 0,
        .type = render::texture::Type::Texture2D,
        .has_mipmap = true,
    };

    horizontal_texture = txt_mgr->create_2d_from_pixels(name + "-bloom-horizontal", {}, h_txt_info, end_caller);

    auto v_txt_info = h_txt_info;
    v_txt_info.width >>= 1;
    v_txt_info.height >>= 1;
    vertical_texture = txt_mgr->create_2d_from_pixels(name + "-bloom-vertical", {}, v_txt_info, end_caller);

    {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = render::texture::Attachment2D { .txt = horizontal_texture };
        source_target = e.get_texture_manager()->create_target(
            name + "-bloom-source", std::move(attachments), end_caller);
    }

    {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = render::texture::Attachment2D { .txt = vertical_texture };
        prefilter_target = e.get_texture_manager()->create_target(
            name + "-bloom-prefilter", std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < horizontal_targets.size(); ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = render::texture::Attachment2D { .txt = horizontal_texture };
        attachments[0].mipmap_level = static_cast<unsigned int>(i + 1);
        horizontal_targets[i] = e.get_texture_manager()->create_target(
            std::format("{}-bloom-horizontal-{}", name, i), std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < vertical_targets.size(); ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = render::texture::Attachment2D { .txt = vertical_texture };
        attachments[0].mipmap_level = static_cast<unsigned int>(i + 1);
        vertical_targets[i] = e.get_texture_manager()->create_target(
            std::format("{}-bloom-vertical-{}", name, i), std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 1; ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = render::texture::Attachment2D { .txt = vertical_texture };
        attachments[0].mipmap_level = static_cast<unsigned int>((GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 2) - i);
        upsampler_targets[i] = e.get_texture_manager()->create_target(
            std::format("{}-bloom-upsampler-{}", name, i), std::move(attachments), end_caller);
    }

    {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = target->get_attachments()[0].var;
        upsampler_targets[GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 1] = e.get_texture_manager()->create_target(
            std::format("{}-bloom-upsampler-{}", name, GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - 1),
            std::move(attachments),
            end_caller);
    }

    GX_LOG_D("BLoom have been created.");
}

gearoenix::render::camera::BloomData::BloomData(BloomData&&) noexcept = default;

gearoenix::render::camera::BloomData::~BloomData() noexcept = default;