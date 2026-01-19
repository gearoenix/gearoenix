#include "gx-vk-scn-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "../gx-vk-marker.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"
#include "gx-vk-scn-scene.hpp"

#include <boost/container/flat_set.hpp>

#include <atomic>

namespace {
std::vector<GxShaderDataScene> shader_datas;
std::atomic<std::uint32_t> shader_data_last_index = 0;
std::shared_ptr<gearoenix::vulkan::buffer::Uniform> uniform_buffer;
boost::container::flat_set<std::pair<double, gearoenix::render::scene::Scene*>> scenes;
}

gearoenix::vulkan::scene::Manager::Manager(): Singleton<Manager>(this)
{
    core::ecs::ComponentType::add<Scene>();

    shader_datas.resize(Scene::max_count);
    uniform_buffer = buffer::Manager::get().create_uniform(sizeof(GxShaderDataScene) * Scene::max_count);
}

gearoenix::vulkan::scene::Manager::~Manager() = default;

gearoenix::core::ecs::EntityPtr gearoenix::vulkan::scene::Manager::build(std::string&& name, double layer) const
{
    auto entity = render::scene::Manager::build(std::move(name), layer);
    entity->add_component(core::Object::construct<Scene>(entity.get(), entity->get_object_name() + "-scene", layer));
    return entity;
}

std::pair<GxShaderDataScene*, std::uint32_t> gearoenix::vulkan::scene::Manager::get_shader_data()
{
    const auto index = shader_data_last_index.fetch_add(1, std::memory_order_relaxed);
    GX_ASSERT_D(index < Scene::max_count);
    return {&shader_datas[index], index};
}

void gearoenix::vulkan::scene::Manager::update() const
{
    shader_data_last_index.store(0, std::memory_order_relaxed);

    render::scene::Manager::update();

    uniform_buffer->update(shader_datas.data());
}

void gearoenix::vulkan::scene::Manager::submit(command::Buffer& cmd)
{
    scenes.clear();
    core::ecs::World::get().synchronised_system<Scene>([&](const auto* const, auto* const scene) -> void {
        if (!scene->get_enabled()) {
            return;
        }
        scenes.emplace(scene->get_layer(), scene);
    });

    // TODO: render shadows
    // TODO: render reflection probes

    if (render::engine::Engine::get().get_specification().is_deferred_supported) {
        // render_with_deferred();
        GX_UNIMPLEMENTED;
    } else {
        render_forward(cmd);
    }

}

void gearoenix::vulkan::scene::Manager::render_forward(command::Buffer& cmd)
{
    auto vk_cmd = cmd.get_vulkan_data();

    {
        GX_VK_PUSH_DEBUG_GROUP(vk_cmd, 1.0f, 0.0f, 0.0f, "render-forward");

        // upload all uniform buffers

        // bind all bindless descriptors

        for (const auto& scene : scenes) {
            scene.second->render_forward();
        }
    }

    const auto& base_os_app = platform::Application::get().get_base();
    const auto& window_size = base_os_app.get_window_size();

    push_debug_group("combine-all-cameras");
    ctx::set_framebuffer(0);
    ctx::set_viewport_scissor_clip({ static_cast<sizei>(0), 0, math::Vec2<sizei>(window_size) });
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    const auto screen_ratio = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
    if (screen_ratio < back_buffer_aspect_ratio) {
        const auto screen_height = static_cast<sizei>(static_cast<float>(window_size.x) / back_buffer_aspect_ratio + 0.1f);
        const auto screen_y = (static_cast<sizei>(window_size.y) - screen_height) / 2;
        ctx::set_viewport_scissor_clip({ static_cast<sizei>(0), screen_y, static_cast<sizei>(window_size.x), screen_height });
    } else {
        const auto screen_width = static_cast<sizei>(static_cast<float>(window_size.y) * back_buffer_aspect_ratio + 0.1f);
        const auto screen_x = (static_cast<sizei>(window_size.x) - screen_width) / 2;
        ctx::set_viewport_scissor_clip({ screen_x, static_cast<sizei>(0), screen_width, static_cast<sizei>(window_size.y) });
    }

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    final_shader->bind(current_shader);
    glActiveTexture(GL_TEXTURE0 + static_cast<enumerated>(final_shader->get_albedo_index()));
    for (auto* const scene : scenes | std::views::values) {
        const auto& record_cameras = scene->get_record().cameras;
        for (auto& camera_index : record_cameras.mains | std::views::values) {
            auto& camera = *core::cast_ptr<Camera>(record_cameras.cameras[camera_index].camera);
            glBindTexture(GL_TEXTURE_2D, camera.get_gl_target().get_default().colour_attachments[1]);
            glBindVertexArray(screen_vertex_object);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    pop_debug_group();
}

const gearoenix::vulkan::buffer::Uniform& gearoenix::vulkan::scene::Manager::get_uniform_buffer()
{
    return *uniform_buffer;
}

#endif