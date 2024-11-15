#include "gx-rnd-lt-directional.hpp"

#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-frustum.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-lt-builder.hpp"

namespace {
const auto directional_allocator = gearoenix::core::allocator::SharedArray<gearoenix::render::light::Directional, gearoenix::render::light::Directional::MAX_COUNT>::construct();
}

gearoenix::render::light::Directional::Directional(std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Light(create_this_type_index(this), std::move(name), entity_id)
    , direction(0.0f, 0.0f, -1.0f)
{
}

const gearoenix::core::ecs::Component::HierarchyTypes& gearoenix::render::light::Directional::get_hierarchy_types() const
{
    static const auto types = generate_hierarchy_types<Light>(this);
    return types;
}

std::shared_ptr<gearoenix::render::light::Directional> gearoenix::render::light::Directional::construct(
    std::string&& name,
    const core::ecs::entity_id_t entity_id)
{
    auto self = directional_allocator->make_shared(std::move(name), entity_id);
    self->set_component_self(self);
    return self;
}

gearoenix::render::light::Directional::~Directional() = default;

gearoenix::render::light::ShadowCasterDirectional::ShadowCasterDirectional(
    const std::type_index final_type_index, std::string&& name,
    const core::ecs::entity_id_t entity_id)
    : Light(final_type_index, std::move(name), entity_id)
{
}

gearoenix::render::light::ShadowCasterDirectional::~ShadowCasterDirectional()
{
    if (nullptr == shadow_camera) {
        return;
    }
    auto* const world = shadow_camera->e.get_world();
    if (nullptr == world) {
        return;
    }
    world->delayed_delete_entity(shadow_camera_entity_id, core::job::EndCaller([] { }));
}

void gearoenix::render::light::ShadowCasterDirectional::initialise(
    engine::Engine& e,
    physics::Transformation* const parent_transform,
    const std::uint32_t resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    const std::shared_ptr<Builder>& builder,
    core::job::EndCaller<>&& end_callback)
{
    if (nullptr == shadow_camera) {
        e.get_camera_manager()->build(
            name + "-shadow-camera", parent_transform,
            core::job::EndCallerShared<camera::Builder>([this, resolution, builder, c = get_component_self(), camera_aspect, camera_near, camera_far, e = std::move(end_callback)](std::shared_ptr<camera::Builder>&& camera_builder) mutable {
                const auto self = c.lock();
                if (nullptr == self) {
                    return;
                }

                shadow_transform = std::dynamic_pointer_cast<physics::Transformation>(camera_builder->get_transformation().get_component_self().lock());
                shadow_frustum = std::dynamic_pointer_cast<physics::collider::Frustum>(camera_builder->get_frustum().get_component_self().lock());

                shadow_camera_entity_id = camera_builder->get_id();
                shadow_camera = camera_builder->get_camera().get_camera_self().lock();
                shadow_camera->set_parent_entity_id(builder->get_id());
                shadow_camera->set_usage(camera::Camera::Usage::Shadow);

                GX_ASSERT_D(nullptr != builder);
                builder->camera_builders[0] = std::move(camera_builder);

                initialise_camera(camera_far, camera_near, camera_aspect);
                set_shadow_map(resolution, std::move(e));
            }),
            core::job::EndCaller([] {}));
    } else {
        initialise_camera(camera_far, camera_near, camera_aspect);
        set_shadow_map(resolution, std::move(end_callback));
    }
}

void gearoenix::render::light::ShadowCasterDirectional::initialise_camera(
    const float camera_far,
    const float camera_near,
    const float camera_aspect)
{
    shadow_camera->set_projection_data(camera::OrthographicProjectionData { .scale = camera_aspect });
    shadow_camera->set_far(camera_far);
    shadow_camera->set_near(camera_near);
}

void gearoenix::render::light::ShadowCasterDirectional::set_shadow_map(
    const std::uint32_t resolution, core::job::EndCaller<>&& end_callback)
{
    if (nullptr != shadow_map && shadow_map->get_info().get_height() == resolution) {
        return;
    }
    auto self = std::dynamic_pointer_cast<ShadowCasterDirectional>(get_component_self().lock());
    GX_ASSERT_D(nullptr != self);
    std::vector<std::uint8_t> pixels0(resolution * resolution * 4);
    std::memset(pixels0.data(), 0, pixels0.size());
    std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
    shadow_camera->e.get_texture_manager()->create_2d_from_pixels(
        name + "-shadow-map",
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

void gearoenix::render::light::ShadowCasterDirectional::set_shadow_map(
    std::shared_ptr<texture::Texture2D>&& sm,
    core::job::EndCaller<>&& end_callback)
{
    auto self = std::dynamic_pointer_cast<ShadowCasterDirectional>(get_component_self().lock());
    shadow_map = std::move(sm);
    auto& e = shadow_map->get_e();
    e.get_texture_manager()->create_target(
        name + "-shadow-map-target",
        { texture::Attachment {
            .mipmap_level = 0,
            .var = texture::Attachment2D {
                .txt = shadow_map,
            } } },
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
