#include "gx-rnd-lt-directional.hpp"

#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-lt-builder.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::render::light::Directional, gearoenix::render::light::Directional::MAX_COUNT> directional_allocator;
}

gearoenix::render::light::Directional::Directional(std::string&& name)
    : Light(create_this_type_index(this), std::move(name))
    , direction(0.0f, 0.0f, -1.0f)
{
}

const boost::container::flat_set<std::type_index>& gearoenix::render::light::Directional::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_type_index<Light>(), create_this_type_index(this) };
    return types;
}

std::shared_ptr<gearoenix::render::light::Directional> gearoenix::render::light::Directional::construct(std::string&& name)
{
    auto self = directional_allocator.make_shared(std::move(name));
    self->set_component_self(self);
    return self;
}

gearoenix::render::light::Directional::~Directional() = default;

gearoenix::render::light::ShadowCasterDirectional::ShadowCasterDirectional(
    const std::type_index final_type_index, std::string&& name)
    : Light(final_type_index, std::move(name))
{
}

gearoenix::render::light::ShadowCasterDirectional::~ShadowCasterDirectional()
{
    if (nullptr == shadow_camera) {
        return;
    }
    shadow_camera->e.get_world()->delayed_delete_entity(shadow_camera_entity_id, core::job::EndCaller([] {}));
}

void gearoenix::render::light::ShadowCasterDirectional::initialise(
    engine::Engine& e,
    const std::size_t resolution,
    const float camera_far,
    const float camera_near,
    const float camera_aspect,
    Builder* const builder,
    core::job::EndCaller<>&& end_callback)
{
    if (nullptr == shadow_map || shadow_map->get_info().height != resolution) {
        auto self = std::dynamic_pointer_cast<ShadowCasterDirectional>(get_component_self().lock());
        GX_ASSERT_D(nullptr != self);
        std::vector<std::uint8_t> pixels0(resolution * resolution * 4);
        std::memset(pixels0.data(), 0, pixels0.size());
        std::vector<std::vector<std::uint8_t>> pixels { std::move(pixels0) };
        e.get_texture_manager()->create_2d_from_pixels(
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
            core::job::EndCallerShared<texture::Texture2D>([e = std::move(end_callback), self = std::move(self)](std::shared_ptr<texture::Texture2D>&& t) mutable {
                self->set_shadow_map(std::move(t), std::move(e));
            }));
    }

    if (nullptr == shadow_camera) {
        auto camera_builder = e.get_camera_manager()->build(name + "-shadow-camera", core::job::EndCaller([] {}));
        shadow_camera_entity_id = camera_builder->get_id();
        shadow_camera = camera_builder->get_camera().get_camera_self().lock();
        shadow_camera->set_parent_entity_id(builder->get_id());
        shadow_camera->set_usage(camera::Camera::Usage::Shadow);

        GX_ASSERT_D(nullptr != builder);
        builder->camera_builders[0] = std::move(camera_builder);
    }
    shadow_camera->set_projection_data(camera::OrthographicProjectionData { .scale = camera_aspect });
    shadow_camera->set_far(camera_far);
    shadow_camera->set_near(camera_near);
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