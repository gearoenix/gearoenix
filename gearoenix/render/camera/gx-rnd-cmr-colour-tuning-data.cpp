#include "gx-rnd-cmr-colour-tuning-data.hpp"

#include "../../platform/gx-plt-file-chooser.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../hdr/gx-rnd-hdr-display-peak.hpp"
#include "../texture/gx-rnd-txt-cube-lut.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-3d.hpp"
#include "gx-rnd-cmr-camera.hpp"

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

float gearoenix::render::camera::ColourTuningData::effective_agx_peak() const
{
    if (track_swapchain_peak) {
        return hdr::agx_peak_ratios[texture::Manager::get().get_active_agx_peak_index()];
    }
    return agx_peak_ratio;
}

gearoenix::render::camera::TonemapMode gearoenix::render::camera::ColourTuningData::effective_tonemap_mode() const
{
    return TonemapMode::CustomLut == tonemap_mode && !colour_lut ? TonemapMode::Identity : tonemap_mode;
}

void gearoenix::render::camera::ColourTuningData::show_debug_gui(const std::weak_ptr<core::Object>& object_self)
{
    if (!ImGui::TreeNode("Colour tuning")) {
        return;
    }
    // Editor-only state for things that aren't camera-side (file import dialog buffer mostly).
    // Shared across cameras since the editor UI is single-threaded and only one inspector is
    // active at a time -- a known limitation.
    //
    // `custom_stream` is the sandbox-safe handle the file picker hands us; on WASM the path
    // string is *not* re-openable by the app, so we keep the stream from Browse alive until the
    // user clicks Import (or browses again, replacing it). On native platforms the stream is
    // strictly redundant, but using one code path keeps WASM working.
    static bool log_encoded_input = true;
    static std::string custom_path;
    static std::shared_ptr<platform::stream::Stream> custom_stream;
    static std::string import_status;

    // Order MUST match the `TonemapMode` enum values (`preset_index` is cast directly).
    constexpr const char* preset_names[] = { "Identity", "AgX", "ACES (Narkowicz)", "Custom (.cube import)" };
    int preset_index = static_cast<int>(tonemap_mode);

    if (ImGui::Combo("Preset", &preset_index, preset_names, IM_ARRAYSIZE(preset_names))) {
        tonemap_mode = static_cast<TonemapMode>(preset_index);
        import_status.clear();
        if (tonemap_mode != TonemapMode::CustomLut) {
            colour_lut.reset(); // Free the bindless slot when leaving custom mode.
        }
    }

    if (tonemap_mode == TonemapMode::AgX) {
        ImGui::Checkbox("Track swapchain peak", &track_swapchain_peak);
        int agx_peak_index = 0;
        if (track_swapchain_peak) {
            agx_peak_index = static_cast<int>(texture::Manager::get().get_active_agx_peak_index());
            ImGui::BeginDisabled();
            ImGui::SliderInt("AgX peak (auto)", &agx_peak_index, 0, static_cast<int>(hdr::agx_peak_ratios.size()) - 1, "%d");
            ImGui::EndDisabled();
        } else {
            // Pre-fill the slider from the current `agx_peak_ratio` so the user starts at the
            // live value when they untick "Track swapchain peak". Out-of-range values clamp to
            // the highest variant.
            agx_peak_index = static_cast<int>(hdr::agx_peak_ratios.size()) - 1;
            for (std::size_t i = 0; i < hdr::agx_peak_ratios.size(); ++i) {
                if (hdr::agx_peak_ratios[i] >= agx_peak_ratio) { agx_peak_index = static_cast<int>(i); break; }
            }
            if (ImGui::SliderInt("AgX peak", &agx_peak_index, 0, static_cast<int>(hdr::agx_peak_ratios.size()) - 1, "%d")) {
                agx_peak_ratio = hdr::agx_peak_ratios[static_cast<std::size_t>(agx_peak_index)];
            }
        }
        ImGui::Text("Peak ratio: %.2fx SDR white", static_cast<double>(effective_agx_peak()));
    }

    if (tonemap_mode == TonemapMode::CustomLut) {
        ImGui::Text("Current: %s", colour_lut ? colour_lut->get_name().c_str() : "<none, click Import below>");
        ImGui::InputText("Path", &custom_path);
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            // Keep both the path (for a display + cache key) and the stream (the only sandbox-safe
            // way to read content on WASM). See `gx-plt-file-chooser.hpp` for the contract.
            platform::file_chooser_open(
                [](platform::stream::Path&& path, std::shared_ptr<platform::stream::Stream>&& stream) {
                    custom_path = path.get_raw_data();
                    custom_stream = std::move(stream);
                },
                [] { },
                "Import .cube colour LUT",
                ".cube");
        }
        ImGui::Checkbox("Source log-encoded input (engine range)", &log_encoded_input);
        if (ImGui::Button("Import")) {
            if (custom_path.empty()) {
                import_status = "Path is empty.";
            } else {
                // Prefer the stream the file picker handed us -- the only sandbox-safe option on
                // WASM. If the user typed a path manually instead of going through Browse, fall
                // back to opening it directly; that works on native platforms but will fail under
                // the browser sandbox.
                std::shared_ptr<platform::stream::Stream> stream = custom_stream;
                if (!stream) {
                    stream = platform::stream::Stream::open(platform::stream::Path::create_absolute(std::string(custom_path)));
                }
                if (!stream) {
                    import_status = "Could not open the file (on WASM you must use Browse...).";
                } else {
                    const auto log_encoding = log_encoded_input
                        ? texture::CubeInputLogEncoding::EngineRange
                        : texture::CubeInputLogEncoding::None;
                    const auto path = platform::stream::Path::create_absolute(std::string(custom_path));
                    texture::TextureInfo info;
                    info.set_type(texture::Type::Texture3D);
                    info.set_format(texture::TextureFormat::RgbaFloat16);
                    info.set_sampler_info(
                        texture::SamplerInfo()
                            .set_min_filter(texture::Filter::Linear)
                            .set_mag_filter(texture::Filter::Linear)
                            .set_wrap_s(texture::Wrap::ClampToEdge)
                            .set_wrap_t(texture::Wrap::ClampToEdge)
                            .set_wrap_r(texture::Wrap::ClampToEdge));
                    // Capture the camera so it stays alive across the async import. After the LUT
                    // lands, write back into its `colour_tuning.colour_lut` -- no risk of dangling
                    // because `cam` keeps the owning Camera alive while the EndCallerShared is in
                    // flight, which transitively keeps this struct alive.
                    const auto cam = std::static_pointer_cast<Camera>(object_self.lock());
                    texture::Manager::get().create_3d_from_cube_file(
                        path, *stream, log_encoding, info,
                        core::job::EndCallerShared<texture::Texture3D>([cam](std::shared_ptr<texture::Texture3D>&& lut) {
                            cam->get_colour_tuning().colour_lut = std::move(lut);
                        }));
                    import_status = "Importing...";
                    custom_stream.reset(); // release after the manager has read its content
                }
            }
        }
        if (!import_status.empty()) {
            ImGui::TextUnformatted(import_status.c_str());
        }
    }
    ImGui::TreePop();
}
