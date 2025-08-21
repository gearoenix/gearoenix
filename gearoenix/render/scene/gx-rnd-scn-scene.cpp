#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Scene::Scene(core::ecs::Entity* const entity, const core::object_type_index_t final_type_index, const double layer, std::string&& name)
    : Component(entity, final_type_index, std::move(name))
    , ssao_settings(0.08f, math::Numeric::epsilon<float>, 0.000f, 0.003f)
    , layer(layer)
    , shadow_render_pass_name("render-shadows for scene: " + object_name)
    , shadow_reflection_probe_render_pass_name("render-reflection-probes for scene: " + object_name)
{
}

gearoenix::render::scene::Scene::~Scene() = default;

void gearoenix::render::scene::Scene::update()
{
    record.update(entity);
}
