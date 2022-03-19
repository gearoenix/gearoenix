#include "gx-rnd-scn-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../camera/gx-rnd-cmr-builder.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../model/gx-rnd-mdl-builder.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Builder::Builder(engine::Engine& e, const std::string& name, const double layer) noexcept
    : entity_builder(e.get_world()->create_shared_builder())
{
    auto& b = entity_builder->get_builder();
    b.add_component(Scene(e, layer));
    b.set_name(name);
}

gearoenix::render::scene::Builder::~Builder() noexcept = default;

void gearoenix::render::scene::Builder::add(std::shared_ptr<model::Builder>&& model_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& mb = model_builder->get_entity_builder()->get_builder();
    auto* m = mb.get_component<model::Model>();
    GX_ASSERT_D(nullptr != m);
    b.get_component<Scene>()->add_model(mb.get_id(), *m);
    m->scene_id = b.get_id();
    model_builders.push_back(std::move(model_builder));
}

void gearoenix::render::scene::Builder::add(std::shared_ptr<camera::Builder>&& camera_builder) noexcept
{
    auto& b = entity_builder->get_builder();
    auto& cb = camera_builder->get_entity_builder()->get_builder();
    auto* c = cb.get_component<camera::Camera>();
    GX_ASSERT_D(nullptr != c);
    b.get_component<Scene>()->add_camera(cb.get_id(), *c);
    c->set_scene_id(b.get_id());
    camera_builders.push_back(std::move(camera_builder));
}
