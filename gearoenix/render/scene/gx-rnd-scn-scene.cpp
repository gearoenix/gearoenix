#include "gx-rnd-scn-scene.hpp"

namespace {
constexpr std::uint32_t io_unique_marker = 0x6056f3a0;
}

gearoenix::render::scene::Scene::Scene(core::ecs::Entity* const entity, const core::object_type_index_t final_type_index, const double layer, std::string&& name)
    : Component(entity, final_type_index, std::move(name))
    , ssao_settings(0.08f, math::Numeric::epsilon<float>, 0.000f, 0.003f)
    , layer(layer)
    , forward_render_pass_name("forward render for scene: " + object_name)
    , shadow_render_pass_name("render-shadows for scene: " + object_name)
    , shadow_reflection_probe_render_pass_name("render-reflection-probes for scene: " + object_name)
{
}

gearoenix::render::scene::Scene::Scene(const core::object_type_index_t final_type_index, const core::object_id_t id, std::string&& name)
    : Component(nullptr, final_type_index, id, std::move(name))
    , ssao_settings(0.08f, math::Numeric::epsilon<float>, 0.000f, 0.003f)
    , forward_render_pass_name("forward render for scene: " + object_name)
    , shadow_render_pass_name("render-shadows for scene: " + object_name)
    , shadow_reflection_probe_render_pass_name("render-reflection-probes for scene: " + object_name)
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

void gearoenix::render::scene::Scene::update() { record.update(entity); }
