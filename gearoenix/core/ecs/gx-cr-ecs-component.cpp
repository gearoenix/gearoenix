#include "gx-cr-ecs-component.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include "../gx-cr-string.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include "../macro/gx-cr-mcr-stringifier.hpp"
#include "gx-cr-ecs-comp-type.hpp"
#include "gx-cr-ecs-world.hpp"
#include <imgui/imgui.h>

void gearoenix::core::ecs::Component::write_in_io_context(std::shared_ptr<platform::stream::Stream>&&, job::EndCaller<>&&) const
{
    GX_UNIMPLEMENTED; // implement it for your final type
}

void gearoenix::core::ecs::Component::update_in_io_context(std::shared_ptr<platform::stream::Stream>&&, job::EndCaller<>&&)
{
    GX_UNIMPLEMENTED; // implement it for your final type
}

gearoenix::core::ecs::Component::Component(const component_index_t final_type_index, std::string&& name, const entity_id_t entity_id)
    : final_type_index(final_type_index)
    , name(std::move(name))
    , entity_id(entity_id)
{
    GX_ASSERT_D(ComponentType::check(final_type_index));
}

void gearoenix::core::ecs::Component::show_debug_gui()
{
    if (!ImGui::TreeNode(String::ptr_name(this).c_str())) {
        return;
    }

    if (!ImGui::BeginTable("##gearoenix::core::ecs::Component", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        return;
    }
    ImGui::TableSetupColumn("##labels", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("##inputs", ImGuiTableColumnFlags_WidthStretch, 0.999f);

    ImGui::TableNextColumn();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(enabled), &enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Name:");
    ImGui::TableNextColumn();
    ImGui::Text("%s", name.c_str());
    ImGui::TableNextColumn();

    ImGui::Text("Final Type: ");
    ImGui::TableNextColumn();
    ImGui::Text("%s", ComponentType::get_name(final_type_index).c_str());
    ImGui::TableNextColumn();

    ImGui::Text("Entity ID:");
    ImGui::TableNextColumn();
    ImGui::Text("%u", entity_id);

    ImGui::EndTable();

    if (const auto* const entity_ptr = World::get()->get_entity(entity_id); nullptr != entity_ptr && ImGui::TreeNode("Entity")) {
        entity_ptr->show_debug_gui();
        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void gearoenix::core::ecs::Component::write(
    std::shared_ptr<platform::stream::Stream>&& stream,
    job::EndCaller<>&& end_callback) const
{
    job::send_job_to_pool([self = component_self.lock(), s = std::move(stream), e = std::move(end_callback)]() mutable {
        s->write_fail_debug(self->final_type_index);
        s->write_fail_debug(self->name);
        s->write_fail_debug(self->enabled);
        s->write_fail_debug(self->entity_id);
        self->write_in_io_context(std::move(s), std::move(e));
    });
}

void gearoenix::core::ecs::Component::update(
    std::shared_ptr<platform::stream::Stream>&& s,
    job::EndCaller<>&& e)
{
    job::send_job_to_pool([self = component_self.lock(), s = std::move(s), e = std::move(e)]() mutable {
        GX_ASSERT(self->final_type_index == s->read<component_index_t>());
        std::string name;
        s->read(name);
        GX_ASSERT(self->name == name);
        s->read(self->enabled);
        GX_ASSERT(self->entity_id == s->read<decltype(self->entity_id)>());
        self->update_in_io_context(std::move(s), std::move(e));
    });
}

void gearoenix::core::ecs::Component::read(
    std::shared_ptr<platform::stream::Stream>&& s, job::EndCallerShared<Component>&& e)
{
    job::send_job_to_pool([s = std::move(s), e = std::move(e)]() mutable {
        const auto final_type_index = s->read<component_index_t>();
        std::string name;
        s->read(name);
        const auto enabled = s->read<bool>();
        const auto entity_id = s->read<entity_id_t>();
        job::EndCallerShared<Component> end([e = std::move(e), enabled](std::shared_ptr<Component>&& c) mutable {
            c->enabled = enabled;
            e.set_return(std::move(c));
        });
        ComponentType::get_stream_constructor(final_type_index)(std::move(name), entity_id, std::move(s), std::move(end));
    });
}

gearoenix::core::ecs::component_index_t gearoenix::core::ecs::Component::read_final_type_index(platform::stream::Stream& s)
{
    const auto final_type_index = s.read<component_index_t>();
    s.seek(s.tell() - sizeof(final_type_index));
    return final_type_index;
}
