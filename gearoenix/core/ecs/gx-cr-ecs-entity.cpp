#include "gx-cr-ecs-entity.hpp"
#include "../../platform/stream/gx-plt-stm-memory.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui.h>

gearoenix::core::ecs::Entity::Entity(
    Archetype* const archetype,
    std::shared_ptr<Component>* const components,
    std::string&& name)
    : archetype(archetype)
    , components(components)
    , name(std::move(name))
{
}

gearoenix::core::ecs::Entity::Entity(Entity&& o) noexcept
    : archetype(o.archetype)
    , components(o.components)
    , name(std::move(o.name))
{
    o.archetype = nullptr;
    o.components = nullptr;
}

std::atomic<gearoenix::core::ecs::entity_id_t> gearoenix::core::ecs::Entity::last_id(1028);

gearoenix::core::ecs::Entity::~Entity() = default;

void gearoenix::core::ecs::Entity::show_debug_gui() const
{
    if (!ImGui::TreeNode(name.c_str())) {
        return;
    }
    static boost::container::flat_set<std::uint32_t> indices;
    indices.clear();
    for (const auto& ci : archetype->components_indices) {
        indices.emplace(ci.second);
    }
    for (const auto i : indices) {
        components[i]->show_debug_gui();
    }
    ImGui::TreePop();
}

void gearoenix::core::ecs::Entity::write(
    std::shared_ptr<platform::stream::Stream>&& stream,
    job::EndCaller<>&& end_callback) const
{
    job::send_job_to_pool([this, s = std::move(stream), e = std::move(end_callback)]() mutable {
        boost::container::flat_set<std::uint32_t> indices;
        for (const auto& ci : archetype->components_indices) {
            indices.emplace(ci.second);
        }
        auto streams = std::make_shared<std::vector<std::shared_ptr<platform::stream::Stream>>>(indices.size());
        job::EndCaller end([n = name, s = std::move(s), e = std::move(e), ss = streams] {
            s->write_fail_debug(n);
            s->write_fail_debug(static_cast<std::uint32_t>(ss->size()));
            for (const auto& ms : *ss) {
                s->write(*ms);
            }
        });
        for (const auto i : indices) {
            auto ms = std::make_shared<platform::stream::Memory>();
            (*streams)[i] = ms;
            components[i]->write(std::move(ms), job::EndCaller(end));
        }
    });
}