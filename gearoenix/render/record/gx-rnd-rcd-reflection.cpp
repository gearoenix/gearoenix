#include "gx-rnd-rcd-reflection.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../physics/accelerator/gx-phs-acc-bvh.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../model/gx-rnd-mdl-uniform.hpp"
#include "../reflection/gx-rnd-rfl-baked.hpp"
#include "../reflection/gx-rnd-rfl-manager.hpp"
#include "gx-rnd-rcd-model.hpp"

#include <ranges>

void gearoenix::render::record::Reflections::update(core::ecs::Entity* const scene_entity)
{
    data.clear();
    core::ecs::World::get().synchronised_system<reflection::Probe>([&](auto* const entity, auto* const probe) {
        if (!probe->get_enabled() || !entity->contains_in_parents(scene_entity)) {
            return;
        }
        probe->update_uniform();
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
    auto* const black_probe = reflection::Manager::get().get_black().get();
    const auto end = static_cast<std::ptrdiff_t>(models.dynamic_models_starting_index);
    for (std::ptrdiff_t i = 0; i < end; ++i) {
        models.models[static_cast<std::size_t>(i)].probe = black_probe;
    }
    update_models(models.static_models_bvh, models.models);
    for (std::ptrdiff_t i = 0; i < end; ++i) {
        auto& m = models.models[static_cast<std::size_t>(i)];
        m.model->get_uniform().template get_ref<GxShaderDataModel>().reflection_probe_bones_begin_reserved_reserved.x
            = m.probe->get_uniform().shader_data_index;
    }
}

void gearoenix::render::record::Reflections::update_dynamic_models(Models& models)
{
    auto* const black_probe = reflection::Manager::get().get_black().get();
#if GEAROENIX_RENDER_RECORD_MODEL_BUILD_BVH_FOR_DYNAMICS
    for (std::size_t i = models.dynamic_models_starting_index; i < models.models.size(); ++i) {
        models.models[i].probe = black_probe;
    }
    update_models(models.dynamic_models_bvh, models.models);
    for (std::size_t i = models.dynamic_models_starting_index; i < models.models.size(); ++i) {
        auto& m = models.models[i];
        m.model->get_uniform().template get_ref<GxShaderDataModel>().reflection_probe_bones_begin_reserved_reserved.x
            = m.probe->get_uniform().shader_data_index;
    }
#else
    const auto begin = models.models.begin() + static_cast<decltype(models.models)::difference_type>(models.dynamic_models_starting_index);
    core::sync::parallel_for(begin, models.models.end(), [&](auto& model, auto) {
        model.probe = black_probe;
        for (const auto& d : data | std::views::values) {
            const auto& box = d.probe->get_include_box();
            if (!box.check_intersection(model.collider->get_surrounding_box())) {
                continue;
            }
            if (box.get_volume() >= model.probe->get_include_box().get_volume()) {
                continue;
            }
            model.probe = d.probe;
        }
        model.model->get_uniform().template get_ref<GxShaderDataModel>().reflection_probe_bones_begin_reserved_reserved.x
            = model.probe->get_uniform().shader_data_index;
    });
#endif
}