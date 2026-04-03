#include "gx-rnd-rcd-reflection.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../reflection/gx-rnd-rfl-probe.hpp"
#include "gx-rnd-rcd-model.hpp"

void gearoenix::render::record::Reflections::update(core::ecs::Entity* const scene_entity)
{
    data.clear();
    core::ecs::World::get().synchronised_system<reflection::Probe>([&](auto* const entity, auto* const probe) {
        if (!probe->get_enabled() || !entity->contains_in_parents(scene_entity)) {
            return;
        }
        data.emplace(entity, Data { .probe = probe });
    });
}

void gearoenix::render::record::Reflections::update_models(const Data& d, physics::accelerator::Bvh& bvh, std::vector<Model>& models)
{
    const auto& box = d.probe->get_include_box();
    bvh.call_on_intersecting(box, [&](const auto& node) {
        auto& m = models[node.index];
        if (box.get_volume() >= m.probe->get_include_box().get_volume()) {
            return;
        }
        m.probe = d.probe;
    });
}

void gearoenix::render::record::Reflections::update_models(physics::accelerator::Bvh& bvh, std::vector<Model>& models)
{
    for (const auto& d : data) {
        update_models(d.second, bvh, models);
    }
}

void gearoenix::render::record::Reflections::update_static_models(Models& models)
{
    update_models(models.static_models_bvh, models.models);
}

void gearoenix::render::record::Reflections::update_dynamic_models(Models& models)
{
    update_models(models.dynamic_models_bvh, models.models);
}