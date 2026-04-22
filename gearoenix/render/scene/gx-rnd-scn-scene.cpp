#include "gx-rnd-scn-scene.hpp"
#include "../../core/gx-cr-profiler.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../buffer/gx-rnd-buf-uniform.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-rnd-scn-uniform.hpp"

namespace {
constexpr std::uint32_t io_unique_marker = 0x6056f3a0;
}

gearoenix::render::scene::Scene::Scene(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_type_index,
    const core::fp_t layer,
    std::string&& name,
    std::shared_ptr<texture::Texture2D>&& brdflut)
    : Component(entity, final_type_index, std::move(name))
    , ssao_settings(0.08f, math::Numeric::epsilon<float>, 0.000f, 0.003f)
    , ambient_light(0.02, 0.02, 0.02)
    , layer(layer)
    , uniform(buffer::Manager::get_range(buffer::UniformRegionIndex::scenes))
    , brdflut(std::move(brdflut))
{
}

void gearoenix::render::scene::Scene::read(std::shared_ptr<Scene>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end)
{
    Component::read(std::shared_ptr<Component>(self), std::shared_ptr(stream), std::move(object_streamer), std::move(end));
    [[maybe_unused]] const auto marker = stream->read<std::remove_cvref_t<decltype(io_unique_marker)>>();
    GX_ASSERT_D(marker == io_unique_marker);
    self->ssao_settings.read(*stream);
    stream->read(self->layer);
}

void gearoenix::render::scene::Scene::write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<core::ObjectStreamer>&& object_streamer, core::job::EndCaller<>&& end_caller)
{
    Component::write(std::shared_ptr(stream), std::move(object_streamer), std::move(end_caller));
    stream->write_fail_debug(io_unique_marker);
    ssao_settings.write(*stream);
    stream->write_fail_debug(layer);
}

gearoenix::render::scene::Scene::~Scene() = default;

void gearoenix::render::scene::Scene::update_uniform()
{
    auto& ref = uniform.get_ref<GxShaderDataScene>();
    ref.ssao_settings = ssao_settings;
    ref.ambient_light_layer = { ambient_light, static_cast<float>(layer) };
    ref.brdflut_sheen_lut_texture_sampler_index.x = brdflut->get_shader_resource_index();
    ref.brdflut_sheen_lut_texture_sampler_index.y = brdflut->get_sampler_shader_resource_index();
}

void gearoenix::render::scene::Scene::update_per_frame()
{
    if (changed) {
        GX_PROFILE_EXP(record.update_after_change(entity));
    }
    update_uniform();
    GX_PROFILE_EXP(record.update_per_frame(entity));
    changed = false;
}

void gearoenix::render::scene::Scene::update_after_change()
{
    changed = true;
}
