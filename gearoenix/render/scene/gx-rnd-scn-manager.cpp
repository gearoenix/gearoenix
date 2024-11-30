#include "gx-rnd-scn-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-scn-builder.hpp"
#include "gx-rnd-scn-scene.hpp"

gearoenix::render::scene::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::Component::register_type<Scene>();
}

gearoenix::render::scene::Manager::~Manager() = default;

std::shared_ptr<gearoenix::render::scene::Builder> gearoenix::render::scene::Manager::build(
    const std::string& name, const double layer, core::job::EndCaller<>&& end_callback) const
{
    return std::make_shared<Builder>(e, name, layer, std::move(end_callback));
}

void gearoenix::render::scene::Manager::update() const
{
    e.get_world()->parallel_system<Scene>([](const core::ecs::entity_id_t scene_id, Scene* const s, const auto /*kernel_index*/) {
        s->update(scene_id);
    });
}
