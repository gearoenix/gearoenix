#include "gx-rnd-lt-directional.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::light::Directional::Directional(std::string&& name)
    : Light(core::ecs::ComponentType::create_index(this), std::move(name))
    , direction(0.0f, 0.0f, -1.0f)
{
}

gearoenix::render::light::Directional::~Directional() = default;

gearoenix::render::light::ShadowCasterDirectional::ShadowCasterDirectional(
    const core::object_type_index_t final_type_index, std::string&& name)
    : Light(final_type_index, std::move(name))
{
}

gearoenix::render::light::ShadowCasterDirectional::~ShadowCasterDirectional() = default;

void gearoenix::render::light::ShadowCasterDirectional::initialise(
    const std::uint32_t resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    core::job::EndCaller<>&& end_callback)
{
    if (nullptr == shadow_camera) {
        camera::Manager::get().build(
            object_name + "-shadow-camera", entity,
            core::job::EndCaller<core::ecs::EntityPtr>([this, resolution, c = object_self, camera_aspect, camera_near, camera_far, e = std::move(end_callback)](core::ecs::EntityPtr&& camera_entity) mutable -> void {
                const auto self = c.lock();
                if (nullptr == self) {
                    return;
                }

                shadow_camera_entity = std::move(camera_entity);

                shadow_transform = shadow_camera_entity->get_component_shared_ptr<physics::Transformation>();
                shadow_frustum = shadow_camera_entity->get_component_shared_ptr<physics::collider::Frustum>();

                shadow_camera = shadow_camera_entity->get_component_shared_ptr<camera::Camera>();
                shadow_camera->set_usage(camera::Camera::Usage::Shadow);

                initialise_camera(camera_far, camera_near, camera_aspect);
                set_shadow_map(resolution, std::move(e));

                (void)self;
            }));
    } else {
        initialise_camera(camera_far, camera_near, camera_aspect);
        set_shadow_map(resolution, std::move(end_callback));
    }
}

void gearoenix::render::light::ShadowCasterDirectional::initialise_camera(const float camera_far, const float camera_near, const float camera_aspect)
{
    shadow_camera->set_projection_data(camera::ProjectionData::construct_orthographic(camera_aspect));
    shadow_camera->set_far(camera_far);
    shadow_camera->set_near(camera_near);
}

void gearoenix::render::light::ShadowCasterDirectional::set_shadow_map(const std::uint32_t resolution, core::job::EndCaller<>&& end_callback)
{
    if (nullptr != shadow_map && shadow_map->get_info().get_height() == resolution) {
        return;
    }
    auto self = std::dynamic_pointer_cast<ShadowCasterDirectional>(object_self.lock());
    GX_ASSERT_D(nullptr != self);
    std::vector<std::uint8_t> pixels0(resolution * resolution * 4);
    std::memset(pixels0.data(), 0, pixels0.size());
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    texture::Manager::get().create_2d_from_pixels(
        object_name + "-shadow-map",
        std::move(pixels),
        texture::TextureInfo()
            .set_format(texture::TextureFormat::D32)
            .set_sampler_info(texture::SamplerInfo()
                    .set_min_filter(texture::Filter::Nearest)
                    .set_mag_filter(texture::Filter::Nearest)
                    .set_wrap_s(texture::Wrap::ClampToEdge)
                    .set_wrap_t(texture::Wrap::ClampToEdge))
            .set_width(resolution)
            .set_height(resolution)
            .set_type(texture::Type::Texture2D)
            .set_has_mipmap(false),
        core::job::EndCallerShared<texture::Texture2D>([e = end_callback, self = std::move(self)](std::shared_ptr<texture::Texture2D>&& t) mutable {
            self->set_shadow_map(std::move(t), std::move(e));
        }));
}

void gearoenix::render::light::ShadowCasterDirectional::set_shadow_map(std::shared_ptr<texture::Texture2D>&& sm, core::job::EndCaller<>&& end_callback)
{
    auto self = std::dynamic_pointer_cast<ShadowCasterDirectional>(object_self.lock());
    shadow_map = std::move(sm);
    texture::Manager::get().create_target(
        object_name + "-shadow-map-target",
        { texture::Attachment(texture::Attachment2D(std::shared_ptr(shadow_map))) },
        core::job::EndCallerShared<texture::Target>([e = std::move(end_callback), s = std::move(self)](std::shared_ptr<texture::Target>&& t) mutable {
            s->set_shadow_map_target(std::move(t));
            (void)e;
        }));
}

void gearoenix::render::light::ShadowCasterDirectional::set_shadow_map_target(std::shared_ptr<texture::Target>&& t)
{
    shadow_map_target = std::move(t);
    shadow_camera->set_customised_target(std::shared_ptr(shadow_map_target));
}
