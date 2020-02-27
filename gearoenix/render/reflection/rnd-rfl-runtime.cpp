#include "rnd-rfl-runtime.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../camera/rnd-cmr-perspective.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../texture/rnd-txt-target.hpp"

gearoenix::render::reflection::Runtime::Runtime(const core::Id id, engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : Reflection(id, Type::Runtime, e)
{
    constexpr texture::Face faces[6] = {
        texture::Face::PositiveX,
        texture::Face::NegativeX,
        texture::Face::PositiveY,
        texture::Face::NegativeY,
        texture::Face::PositiveZ,
        texture::Face::NegativeZ,
    };
    auto* const sys_app = e->get_system_application();
    auto* const cam_mgr = sys_app->get_asset_manager()->get_camera_manager();
    const auto resolution = static_cast<unsigned int>(sys_app->get_configuration().render_config.runtime_reflection_resolution);
    targets[0] = e->create_render_target(
        core::asset::Manager::create_id(),
        { texture::AttachmentInfo {
            .texture_info = texture::TextureInfo {
                .format = texture::TextureFormat::RgbaFloat32,
                .sample_info = texture::SampleInfo {
                    .min_filter = texture::Filter::LinearMipmapLinear,
                    .mag_filter = texture::Filter::Linear,
                    .wrap_s = texture::Wrap::ClampToEdge,
                    .wrap_t = texture::Wrap::ClampToEdge,
                    .wrap_r = texture::Wrap::ClampToEdge,
                },
                .texture_type = texture::Type::TextureCube,
                .has_mipmap = true,
            },
            .img_width = resolution,
            .img_height = resolution,
            .usage = texture::UsageFlag::Color,
            .face = faces[0],
        } },
        call);
    for (int i = 1; i < 6; ++i) {
        targets[i] = std::shared_ptr<texture::Target>(targets[0]->clone());
        targets[i]->set_attachment_face(0, faces[i]);
    }

    for (int i = 0; i < 6; ++i) {
        auto& cam = cameras[i];
        cam = cam_mgr->create<camera::Perspective>();
        cam->set_target(targets[i].get());
    }
}

gearoenix::render::reflection::Runtime::~Runtime() noexcept = default;
