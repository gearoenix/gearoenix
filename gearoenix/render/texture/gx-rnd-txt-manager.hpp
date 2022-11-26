#ifndef GEAROENIX_RENDER_TEXTURE_MANAGER_HPP
#define GEAROENIX_RENDER_TEXTURE_MANAGER_HPP
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-txt-attachment.hpp"
#include "gx-rnd-txt-texture-info.hpp"
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

    [[nodiscard]] virtual std::shared_ptr<Texture2D> create_2d_from_pixels_v(
        std::string name,
        std::vector<std::vector<std::uint8_t>> pixels,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept
        = 0;
    [[nodiscard]] virtual std::shared_ptr<TextureCube> create_cube_from_pixels_v(
        std::string name,
        std::vector<std::vector<std::vector<std::uint8_t>>> pixels,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept
        = 0;
    [[nodiscard]] virtual std::shared_ptr<Target> create_target_v(
        std::string name,
        std::vector<Attachment>&& attachments,
        const core::sync::EndCaller& c) noexcept
        = 0;

public:
    explicit Manager(engine::Engine& e) noexcept;
    virtual ~Manager() noexcept;
    [[nodiscard]] std::shared_ptr<Texture> read_gx3d(
        const platform::stream::Path& path,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture> read_gx3d(
        const std::shared_ptr<platform::stream::Stream>& stream,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2d_from_colour(
        const math::Vec4<float>& colour,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> get_brdflut(
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> get_checker(
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2d_from_pixels(
        std::string name,
        std::vector<std::vector<std::uint8_t>> pixels,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2d_from_formatted(
        std::string name,
        const void* data,
        std::size_t size,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2df_from_formatted(
        std::string name,
        const void* data,
        std::size_t size,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2d_from_file(
        std::string name,
        const platform::stream::Path& path,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<TextureCube> create_cube_from_colour(
        const math::Vec4<float>& colour,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<TextureCube> create_cube_from_pixels(
        std::string name,
        std::vector<std::vector<std::vector<std::uint8_t>>> pixels,
        const TextureInfo& info,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] std::shared_ptr<Target> create_target(
        std::string name,
        std::vector<Attachment>&& attachments,
        const core::sync::EndCaller& c) noexcept;
    [[nodiscard]] static constexpr float geometry_smith(
        const math::Vec3<float>& n,
        const math::Vec3<float>& v,
        const math::Vec3<float>& l,
        float roughness) noexcept;
    [[nodiscard]] static math::Vec2<float> integrate_brdf(
        float n_dot_v,
        float roughness) noexcept;
    [[nodiscard]] static std::vector<math::Vec4<std::uint8_t>> create_brdflut_pixels(
        std::size_t resolution = 256) noexcept;
};
}

constexpr float gearoenix::render::texture::Manager::geometry_smith(
    const math::Vec3<float>& n,
    const math::Vec3<float>& v,
    const math::Vec3<float>& l,
    const float roughness) noexcept
{
    auto temp = n.dot(v);
    const auto n_dot_v = temp < 0.0f ? 0.0f : temp;
    temp = n.dot(l);
    const auto n_dot_l = temp < 0.0f ? 0.0f : temp;
    const auto ggx2 = math::Numeric::geometry_schlick_ggx(n_dot_v, roughness);
    const auto ggx1 = math::Numeric::geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}

#endif
