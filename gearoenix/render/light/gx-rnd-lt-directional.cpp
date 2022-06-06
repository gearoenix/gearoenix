#include "gx-rnd-lt-directional.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "gx-rnd-lt-builder.hpp"

gearoenix::render::light::Directional::Directional() noexcept
    : core::ecs::Component(this)
    , direction(0.0f, 0.0f, -1.0f)
{
}

gearoenix::render::light::Directional::Directional(Directional&&) noexcept = default;

gearoenix::render::light::Directional::~Directional() noexcept = default;

gearoenix::render::light::ShadowCasterDirectional::ShadowCasterDirectional(
    const std::string& name,
    const std::size_t resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    engine::Engine& e,
    Builder& builder,
    const core::sync::EndCallerIgnored& end_callback) noexcept
    : core::ecs::Component(this)
{
    std::vector<std::uint8_t> pixels0(resolution * resolution * 4);
    std::memset(pixels0.data(), 0, pixels0.size());
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    shadow_map = e.get_texture_manager()->create_2d_from_pixels(
        name + "-shadow-map",
        std::move(pixels),
        texture::TextureInfo {
            .format = texture::TextureFormat::D32,
            .sampler_info = texture::SamplerInfo {
                .min_filter = texture::Filter::Nearest,
                .mag_filter = texture::Filter::Nearest,
                .wrap_s = texture::Wrap::ClampToEdge,
                .wrap_t = texture::Wrap::ClampToEdge,
            },
            .width = resolution,
            .height = resolution,
            .type = texture::Type::Texture2D,
            .has_mipmap = false,
        },
        end_callback);
    shadow_map_target = e.get_texture_manager()->create_target(
        name + "-shadow-map-target",
        { texture::Attachment {
            .mipmap_level = 0,
            .var = texture::Attachment2D {
                .txt = shadow_map,
            } } },
        end_callback);
    auto camera_builder = e.get_camera_manager()->build(name + "-shadow-camera");
    camera_id = camera_builder->get_entity_builder()->get_builder().get_id();
    camera_builder->set_target(std::shared_ptr(shadow_map_target));
    auto& cam = camera_builder->get_camera();
    cam.set_projection_type(camera::Projection::Orthographic);
    cam.set_usage(camera::Camera::Usage::Shadow);
    cam.set_far(camera_far);
    cam.set_near(camera_near);
    cam.set_scale(camera_aspect);
    cam.set_reference_id(builder.get_entity_builder()->get_builder().get_id());
    builder.set_cameras({ std::move(camera_builder), nullptr, nullptr, nullptr, nullptr, nullptr });
}

gearoenix::render::light::ShadowCasterDirectional::ShadowCasterDirectional(ShadowCasterDirectional&&) noexcept = default;

gearoenix::render::light::ShadowCasterDirectional::~ShadowCasterDirectional() noexcept = default;
