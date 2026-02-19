#include "gx-vk-rfl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../render/reflection/gx-rnd-rfl-baked.hpp"
#include "../../render/reflection/gx-rnd-rfl-runtime.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../texture/gx-vk-txt-manager.hpp"
#include "../texture/gx-vk-txt-cube.hpp"
#include "gx-vk-rfl-baked.hpp"
#include "gx-vk-rfl-runtime.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"

void gearoenix::vulkan::reflection::Manager::update()
{
    render::reflection::Manager::update();
}

gearoenix::vulkan::reflection::Manager::Manager()
    : Singleton<Manager>(this)
    , uniform_indexer(render::reflection::Baked::max_count + render::reflection::Runtime::max_count)
{
    core::ecs::ComponentType::add<Probe, Runtime>();
    core::ecs::ComponentType::add<Baked>();
    core::ecs::ComponentType::add<Runtime>();

    render::texture::Manager::get().create_cube_from_colour({}, core::job::EndCallerShared<render::texture::TextureCube>([this](std::shared_ptr<render::texture::TextureCube>&& irr) {
        auto rad = irr;
        black = core::Object::construct<Baked>(
            nullptr, "reflection-default-black", std::move(irr), std::move(rad),
            math::Aabb3(math::Vec3(std::numeric_limits<double>::max()), -math::Vec3(std::numeric_limits<double>::max())));
    }));
}

gearoenix::vulkan::reflection::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::reflection::Manager::build_baked(
    std::string&& name,
    core::ecs::Entity* const parent,
    std::shared_ptr<render::texture::TextureCube>&& irradiance,
    std::shared_ptr<render::texture::TextureCube>&& radiance,
    const math::Aabb3<double>& include_box)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    entity->add_component(core::Object::construct<Baked>(entity.get(), entity->get_object_name() + "-gapi-reflection", std::move(irradiance), std::move(radiance), include_box));
    return entity;
}

void gearoenix::vulkan::reflection::Manager::build_runtime(
    std::string&& name,
    core::ecs::Entity* const parent,
    const math::Aabb3<double>& receive_box,
    const math::Aabb3<double>& exclude_box,
    const math::Aabb3<double>& include_box,
    const std::uint32_t environment_resolution,
    const std::uint32_t irradiance_resolution,
    const std::uint32_t radiance_resolution,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    auto entity = core::ecs::Entity::construct(std::move(name), parent);
    auto* const ptr = entity.get();
    entity_callback.set_return(std::move(entity));
    Runtime::construct(
        ptr, receive_box, exclude_box, include_box,
        ptr->get_object_name() + "-gapi-runtime-reflection",
        environment_resolution, irradiance_resolution, radiance_resolution,
        core::job::EndCallerShared<Runtime>([e = std::move(entity_callback)](std::shared_ptr<Runtime>&& r) -> void {
            e.get_return()->add_component(std::move(r));
        }));
}

void gearoenix::vulkan::reflection::Manager::upload_uniforms()
{
    uniform_indexer.update();
}

void gearoenix::vulkan::reflection::Manager::submit(const VkCommandBuffer cmd) const
{
    GX_VK_PUSH_DEBUG_GROUP(cmd, 0.5f, 1.0f, 0.5f, "render-reflection-probes");

    core::ecs::World::get().synchronised_system<Runtime>([&](const auto* const, const auto* const r) {
        constexpr std::array face_uv_axis {
            std::array { math::Vec3(0.0f, 0.0f, -1.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(1.0f, 0.0f, 0.0f) }, // PositiveX
            std::array { math::Vec3(0.0f, 0.0f, 1.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(-1.0f, 0.0f, 0.0f) }, // NegativeX
            std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f), math::Vec3(0.0f, 1.0f, 0.0f) }, // PositiveY
            std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, 0.0f, -1.0f), math::Vec3(0.0f, -1.0f, 0.0f) }, // NegativeY
            std::array { math::Vec3(1.0f, 0.0f, 0.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(0.0f, 0.0f, 1.0f) }, // PositiveZ
            std::array { math::Vec3(-1.0f, 0.0f, 0.0f), math::Vec3(0.0f, -1.0f, 0.0f), math::Vec3(0.0f, 0.0f, -1.0f) }, // NegativeZ
        };
        switch (r->get_state()) {
        case Runtime::State::EnvironmentCubeMipMap:
            r->get_gapi_environment()->generate_mipmap(cmd);
            return;
        case Runtime::State::IrradianceFace: {
            const auto fi = r->get_state_irradiance_face();
            ctx::set_framebuffer(r->get_gl_irradiance_targets_v()[fi]);
            const auto w = static_cast<sizei>(r->get_irradiance()->get_info().get_width());
            ctx::set_viewport_scissor_clip({ 0, 0, w, w });
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            irradiance_shader->bind(current_shader);
            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(irradiance_shader->get_environment_index()));
            glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
            irradiance_shader->set_m_data(reinterpret_cast<const float*>(&face_uv_axis[fi]));
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            return;
        }
        case Runtime::State::IrradianceMipMap:
            glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_irradiance_v());
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            return;
        case Runtime::State::RadianceFaceLevel: {
            const auto fi = r->get_state_radiance_face();
            const auto li = r->get_state_radiance_level();
            ctx::set_framebuffer(r->get_gl_radiance_targets_v()[fi][li]);
            const auto w = static_cast<sizei>(r->get_radiance()->get_info().get_width() >> li);
            ctx::set_viewport_scissor_clip({ 0, 0, w, w });
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            radiance_shader->bind(current_shader);
            glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(radiance_shader->get_environment_index()));
            glBindTexture(GL_TEXTURE_CUBE_MAP, r->get_gl_environment_v());
            radiance_shader->set_m_data(reinterpret_cast<const float*>(&face_uv_axis[fi]));
            const auto roughness = static_cast<float>(r->get_roughnesses()[li]);
            radiance_shader->set_roughness_data(&roughness);
            const float roughness_p_4 = roughness * roughness * roughness * roughness;
            radiance_shader->set_roughness_p_4_data(&roughness_p_4);
            const auto resolution = static_cast<float>(r->get_environment()->get_info().get_width());
            const float sa_texel = (static_cast<float>(std::numbers::pi) / 1.5f) / (resolution * resolution);
            radiance_shader->set_sa_texel_data(&sa_texel);
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            return;
        }
        default:
            return;
        }
    });
}

#endif
