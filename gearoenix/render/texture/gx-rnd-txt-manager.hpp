#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/job/gx-cr-job-single-end.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../gx-rnd-constants.hpp"
#include "gx-rnd-txt-attachment.hpp"
#include "gx-rnd-txt-texture-info.hpp"

#include <array>
#include <functional>
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
struct Path;
}

namespace gearoenix::render::texture {
struct Texture;
struct Texture2D;
struct Texture3D;
struct TextureCube;
struct Target;
enum struct CubeInputLogEncoding : std::uint8_t;

struct DefaultCameraTargets final {
    std::shared_ptr<Target> main;
    std::array<std::array<std::shared_ptr<Target>, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT>, 2> targets;
};

struct Manager : core::Singleton<Manager> {

protected:
    std::shared_ptr<Texture2D> brdflut;
    /// Currently picked AgX peak index (matching `hdr::agx_peak_ratios`). Submission updates this
    /// on HDR-state change (defaults to 0 = SDR until then); cameras read it during dispatch and
    /// push the corresponding `agx_peak_ratio` float to the shader. No observer / no LUT swap
    /// needed -- AgX is computed inline now, so picking a different peak just changes a float.
    std::uint8_t active_agx_peak_index = 0;
    std::shared_ptr<Texture2D> blue_noise;
    std::shared_ptr<Texture2D> checkers;
    core::job::SingleEnd<Texture2D> textures_2d;
    core::job::SingleEnd<Texture3D> textures_3d;
    core::job::SingleEnd<TextureCube> textures_cube;
    core::job::SingleEnd<Target> targets;

    virtual void create_2d_from_pixels_v(std::string&& name, std::vector<std::vector<std::uint8_t>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<Texture2D>&& c) = 0;
    virtual void create_3d_from_pixels_v(std::string&& name, std::vector<std::vector<std::uint8_t>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<Texture3D>&& c) = 0;
    virtual void create_cube_from_pixels_v(std::string&& name, std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<TextureCube>&& c) = 0;
    virtual void create_target_v(std::string&& name, std::vector<Attachment>&& attachments, core::job::EndCallerShared<Target>&& c) = 0;

public:
    Manager();
    ~Manager() override;
    void read_gx3d(const platform::stream::Path& path, core::job::EndCallerShared<Texture>&& c, bool use_cache = true);
    void read_gx3d(platform::stream::Stream& stream, core::job::EndCallerShared<Texture>&& c, bool use_cache = true);
    void create_2d_from_colour(const math::Vec4<float>& colour, core::job::EndCallerShared<Texture2D>&& c, bool use_cache = true);
    void get_brdflut(core::job::EndCallerShared<Texture2D>&& c);
    /// Active AgX peak index, in [0, hdr::agx_peak_ratios.size()). Set by submission on HDR-state
    /// change; read by cameras during dispatch to pick the right peak float. Plain integer (no
    /// observer) -- AgX is computed inline now, so picking a different peak just changes a uint
    /// the next frame.
    [[nodiscard]] std::uint8_t get_active_agx_peak_index() const { return active_agx_peak_index; }
    void set_active_agx_peak_index(std::uint8_t peak_index) { active_agx_peak_index = peak_index; }
    void get_blue_noise(core::job::EndCallerShared<Texture2D>&& c);
    void get_checker(core::job::EndCallerShared<Texture2D>&& c);
    [[nodiscard]] bool get_from_cache(const std::string& name, core::job::EndCallerShared<Texture>&& c, const TextureInfo& info);
    [[nodiscard]] bool get_from_cache(const std::string& name, core::job::EndCallerShared<Texture2D>& c, const TextureInfo* info);
    [[nodiscard]] bool get_from_cache(const std::string& name, core::job::EndCallerShared<Texture3D>& c, const TextureInfo* info);
    [[nodiscard]] bool get_from_cache(const std::string& name, core::job::EndCallerShared<TextureCube>& c, const TextureInfo* info);
    void create_2d_from_pixels(std::string&& name, std::vector<std::vector<std::uint8_t>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<Texture2D>&& c, bool use_cache = true);
    void create_3d_from_pixels(std::string&& name, std::vector<std::vector<std::uint8_t>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<Texture3D>&& c, bool use_cache = true);
    void create_2d_from_formatted(std::string&& name, const void* data, std::uint32_t size, const TextureInfo& info, core::job::EndCallerShared<Texture2D>&& c, bool use_cache = true);
    void create_2df_from_formatted(std::string&& name, const void* data, std::uint32_t size, const TextureInfo& info, core::job::EndCallerShared<Texture2D>&& c, bool use_cache = true);
    void create_2d_from_file(const platform::stream::Path& path, const TextureInfo& info, core::job::EndCallerShared<Texture2D>&& c, bool use_cache = true);
    void create(const platform::stream::Path& path, platform::stream::Stream& stream, const TextureInfo& info, core::job::EndCallerShared<Texture>&& c, bool use_cache = true);
    void create_cube_from_colour(const math::Vec4<float>& colour, core::job::EndCallerShared<TextureCube>&& c, bool use_cache = true);
    void create_cube_from_pixels(std::string&& name, std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels, const TextureInfo& info, core::job::EndCallerShared<TextureCube>&& c, bool use_cache = true);
    void create_target(std::string&& name, std::vector<Attachment>&& attachments, core::job::EndCallerShared<Target>&& c, bool use_cache = true);
    void create_target(std::shared_ptr<platform::stream::Stream>&& stream, core::job::EndCallerShared<Target>&& c, bool use_cache = true);
    [[nodiscard]] static math::Vec2<float> integrate_brdf(float n_dot_v, float roughness);
    [[nodiscard]] static std::vector<math::Vec4<std::uint8_t>> create_brdflut_pixels(std::uint32_t resolution = 256);
    /// Generate a square R8 blue-noise tile via void-and-cluster (Ulichney, 1993). Toroidal.
    [[nodiscard]] static std::vector<std::uint8_t> create_blue_noise_pixels(std::uint32_t side = 256);
    [[nodiscard]] math::Vec2<std::uint32_t> get_default_camera_render_target_dimensions() const;
    void create_default_camera_render_target(const std::string& camera_name, core::job::EndCaller<DefaultCameraTargets>&& callback, bool use_cache = true);

    /// Import a `.cube` LUT and resample it onto the engine's log-input grid. `log_encoding` says
    /// whether the source's input axis is already engine-log-encoded (the only knob exposed to
    /// artists; tonemap is baked into the LUT outputs and the engine is OETF-agnostic at import).
    /// The caller must hand in an already-opened `stream` -- on WASM the file picker yields a
    /// sandboxed blob whose path is not openable later, so re-opening from `path` would fail.
    /// `path` is used purely as the cache key here.
    void create_3d_from_cube_file(
        const platform::stream::Path& path,
        platform::stream::Stream& stream,
        CubeInputLogEncoding log_encoding,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture3D>&& c,
        bool use_cache = true);

    /// Load a 3D texture from the disk. Routes by extension:
    ///   `.gx-3d-texture` -> engine wrapper around a KTX2 payload (uses `read_gx3d`)
    ///   `.ktx2`          -> raw KTX2 file; the caller supplies sampler info via `info`
    ///                       (dimensions / format are read from the file itself)
    ///   `.cube`          -> Adobe `.cube` 3D LUT, parsed into an RGBA16F Texture3D
    /// Any other extension fails fatally.
    void create_3d_from_file(const platform::stream::Path& path, const TextureInfo& info, core::job::EndCallerShared<Texture3D>&& c, bool use_cache = true);
};
}