#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "../gx-rnd-build-configuration.hpp"
#include "gx-rnd-txt-attachment.hpp"
#include "gx-rnd-txt-texture-info.hpp"
#include <array>
#include <map>
#include <memory>

namespace gearoenix::platform::stream {
struct Stream;
struct Path;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct Texture;
struct Texture2D;
struct TextureCube;
struct Target;

struct DefaultCameraTargets final {
    std::shared_ptr<Target> main;
    std::array<std::array<std::shared_ptr<Target>, GX_RENDER_DEFAULT_CAMERA_TARGET_MIPS_COUNT>, 2> targets;
};

struct Manager {

protected:
    engine::Engine& e;
    std::mutex brdflut_lock;
    std::shared_ptr<Texture2D> brdflut;
    std::mutex checkers_lock;
    std::shared_ptr<Texture2D> checkers;
    std::mutex textures_2d_lock;
    std::map<std::string, std::weak_ptr<Texture2D>> textures_2d;
    std::mutex textures_cube_lock;
    std::map<std::string, std::weak_ptr<TextureCube>> textures_cube;
    std::mutex targets_lock;
    std::map<std::string, std::weak_ptr<Target>> targets;

    virtual void create_2d_from_pixels_v(
        std::string&& name,
        std::vector<std::vector<std::uint8_t>>&& pixels,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture2D>&& c)
        = 0;
    virtual void create_cube_from_pixels_v(
        std::string&& name,
        std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels,
        const TextureInfo& info,
        core::job::EndCallerShared<TextureCube>&& c)
        = 0;
    virtual void create_target_v(
        std::string&& name,
        std::vector<Attachment>&& attachments,
        core::job::EndCallerShared<Target>&& c)
        = 0;

public:
    explicit Manager(engine::Engine& e);
    virtual ~Manager();
    void read_gx3d(const platform::stream::Path& path, core::job::EndCallerShared<Texture>&& c);
    void read_gx3d(platform::stream::Stream& stream, core::job::EndCallerShared<Texture>&& c);
    void create_2d_from_colour(const math::Vec4<float>& colour, core::job::EndCallerShared<Texture2D>&& c);
    void get_brdflut(core::job::EndCallerShared<Texture2D>&& c);
    void get_checker(core::job::EndCallerShared<Texture2D>&& c);
    void create_2d_from_pixels(
        std::string&& name,
        std::vector<std::vector<std::uint8_t>>&& pixels,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture2D>&& c);
    void create_2d_from_formatted(
        std::string&& name,
        const void* data,
        std::uint32_t size,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture2D>&& c);
    void create_2df_from_formatted(
        std::string&& name,
        const void* data,
        std::uint32_t size,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture2D>&& c);
    void create_2d_from_file(
        const platform::stream::Path& path,
        const TextureInfo& info,
        core::job::EndCallerShared<Texture2D>&& c);
    void create_cube_from_colour(const math::Vec4<float>& colour, core::job::EndCallerShared<TextureCube>&& c);
    void create_cube_from_pixels(
        std::string&& name,
        std::vector<std::vector<std::vector<std::uint8_t>>>&& pixels,
        const TextureInfo& info,
        core::job::EndCallerShared<TextureCube>&& c);
    void create_target(
        std::string&& name,
        std::vector<Attachment>&& attachments,
        core::job::EndCallerShared<Target>&& c);
    [[nodiscard]] constexpr static float geometry_smith(
        const math::Vec3<float>& n,
        const math::Vec3<float>& v,
        const math::Vec3<float>& l,
        float roughness);
    [[nodiscard]] static math::Vec2<float> integrate_brdf(float n_dot_v, float roughness);
    [[nodiscard]] static std::vector<math::Vec4<std::uint8_t>> create_brdflut_pixels(std::uint32_t resolution = 256);
    [[nodiscard]] math::Vec2<std::uint32_t> get_default_camera_render_target_dimensions() const;
    void create_default_camera_render_target(
        const std::string& camera_name,
        core::job::EndCaller<DefaultCameraTargets>&& callback);
};
}

constexpr float gearoenix::render::texture::Manager::geometry_smith(
    const math::Vec3<float>& n,
    const math::Vec3<float>& v,
    const math::Vec3<float>& l,
    const float roughness)
{
    auto temp = n.dot(v);
    const auto n_dot_v = temp < 0.0f ? 0.0f : temp;
    temp = n.dot(l);
    const auto n_dot_l = temp < 0.0f ? 0.0f : temp;
    const auto ggx2 = math::Numeric::geometry_schlick_ggx(n_dot_v, roughness);
    const auto ggx1 = math::Numeric::geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}