#include "gx-rnd-cmr-bloom-data.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include <imgui.h>

gearoenix::render::camera::BloomData::BloomData(
    engine::Engine& e,
    const std::string& name,
    const core::sync::EndCaller& end_caller,
    const std::shared_ptr<texture::Target>& target,
    const std::shared_ptr<texture::Target>& second_target) noexcept
    : scatter_clamp_max_threshold_threshold_knee(0.5f, 7.0f, 0.9f, 0.5f)
{

    // S0 -> S1 Prefilter
    // S1 -> M1 HG ------ M1 -> S2 VG
    // S2 -> M2 HG ------ M2 -> S3 VG
    // S3 -> M3 HG ------ M3 -> S4 VG
    // S4 -> M4 HG ------ M4 -> S5 VG
    // S5 -> M5 HG ------ M5 -> S6 VG
    // S6 -> M6 HG ------ M6 -> S7 VG
    // S7, S6 -> M6 US
    // M6, S5 -> M5 US
    // M5, S4 -> M4 US
    // M4, S3 -> M3 US
    // M3, S2 -> M2 US
    // M2, S1 -> M1 US
    // M1, S0 -> M0 US

    auto* const txt_mgr = e.get_texture_manager();

    {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = second_target->get_attachments()[0].var;
        attachments[0].mipmap_level = 1;
        prefilter_target = txt_mgr->create_target(
            name + "-bloom-prefilter", std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < horizontal_targets.size(); ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = target->get_attachments()[0].var;
        attachments[0].mipmap_level = static_cast<unsigned int>(i + 1);
        horizontal_targets[i] = txt_mgr->create_target(
            name + "-bloom-horizontal-" + std::to_string(i), std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < vertical_targets.size(); ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = second_target->get_attachments()[0].var;
        attachments[0].mipmap_level = static_cast<unsigned int>(i + 2);
        vertical_targets[i] = txt_mgr->create_target(
            name + "-bloom-vertical-" + std::to_string(i), std::move(attachments), end_caller);
    }

    for (std::size_t i = 0; i < upsampler_targets.size(); ++i) {
        std::vector<render::texture::Attachment> attachments(1);
        attachments[0].var = target->get_attachments()[0].var;
        attachments[0].mipmap_level = static_cast<unsigned int>(GX_RENDER_MAX_BLOOM_DOWN_SAMPLE_COUNT - i);
        upsampler_targets[i] = txt_mgr->create_target(
            name + "-bloom-upsampler-" + std::to_string(i), std::move(attachments), end_caller);
    }

    GX_LOG_D("BLoom have been created.");
}

gearoenix::render::camera::BloomData::BloomData(BloomData&&) noexcept = default;

gearoenix::render::camera::BloomData& gearoenix::render::camera::BloomData::operator=(BloomData&&) noexcept = default;

gearoenix::render::camera::BloomData::~BloomData() noexcept = default;

void gearoenix::render::camera::BloomData::show_debug_data() noexcept
{
    if (ImGui::TreeNode("Bloom")) {
        ImGui::InputFloat("Scatter", &scatter_clamp_max_threshold_threshold_knee.x, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Clamp Max", &scatter_clamp_max_threshold_threshold_knee.y, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Threshold", &scatter_clamp_max_threshold_threshold_knee.z, 0.01f, 1.0f, "%.3f");
        ImGui::InputFloat("Threshold Knee", &scatter_clamp_max_threshold_threshold_knee.w, 0.01f, 1.0f, "%.3f");
        ImGui::TreePop();
    }
}