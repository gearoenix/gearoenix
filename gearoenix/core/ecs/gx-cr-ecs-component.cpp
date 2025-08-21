#include "gx-cr-ecs-component.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-world.hpp"

gearoenix::core::ecs::Component::Component(Entity* const entity, const object_type_index_t final_type_index, std::string&& name)
    : Object(final_type_index, std::move(name))
    , entity(entity)
{
    GX_ASSERT_D(ComponentType::check(final_type_index));
}

gearoenix::core::ecs::Component::Component(Entity* const entity, const object_type_index_t final_type_index, const object_id_t object_id, std::string&& name)
    : Object(final_type_index, object_id, std::move(name))
    , entity(entity)
{
    GX_ASSERT_D(ComponentType::check(final_type_index));
}

void gearoenix::core::ecs::Component::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(enabled);
    s.write_fail_debug(entity->get_object_id());
}

void gearoenix::core::ecs::Component::read(platform::stream::Stream& s)
{
    enabled = s.read<bool>();
    const auto entity_id = s.read<object_id_t>();
    GX_TODO; // Fetch the entity from an object stream.
}

gearoenix::core::ecs::Component::~Component() = default;

void gearoenix::core::ecs::Component::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        Object::show_debug_gui();

        render::imgui::table_scope("##gearoenix::core::ecs::Component", [this] {
            ImGui::Text("Enabled:");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##enabled", &enabled);
            ImGui::TableNextColumn();

            ImGui::Text("Entity ID:");
            ImGui::TableNextColumn();
            ImGui::Text("%u", entity->get_object_id());
        });

        if (nullptr != entity && ImGui::TreeNode("Entity")) {
            entity->show_debug_gui();
            ImGui::TreePop();
        }
    });
}

void gearoenix::core::ecs::Component::set_entity(Entity* const e)
{
    entity = e;
}
