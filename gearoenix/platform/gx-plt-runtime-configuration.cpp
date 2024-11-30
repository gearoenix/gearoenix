#include "gx-plt-runtime-configuration.hpp"

#include <imgui_internal.h>

#include "../core/ecs/gx-cr-ecs-world.hpp"
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/gx-cr-string.hpp"
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "../render/imgui/gx-rnd-imgui-input-uint.hpp"
#include "gx-plt-application.hpp"
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace {
constexpr auto entity_name = "gearoenix-platform-runtime-configuration";
std::shared_ptr<gearoenix::platform::RuntimeConfiguration> default_value = nullptr;
}

gearoenix::platform::RuntimeConfiguration::RuntimeConfiguration(const core::ecs::entity_id_t id)
    : Component(create_this_type_index(this), entity_name, id)
    , application_name(GX_APPLICATION_NAME)
{
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get(core::ecs::World* const w)
{
    if (nullptr == w) {
        if (nullptr == default_value) {
            default_value = std::shared_ptr<RuntimeConfiguration>(new RuntimeConfiguration(core::ecs::INVALID_ENTITY_ID));
        }
        return *default_value;
    }

    RuntimeConfiguration* result = nullptr;
    w->synchronised_system<RuntimeConfiguration>([&](const auto, auto* const ptr) {
        GX_ASSERT_D(nullptr == result); // only one must exist
        result = ptr;
    });

    if (nullptr != result) {
        return *result;
    }

    core::ecs::EntityBuilder builder(entity_name, core::job::EndCaller([] { }));
    if (nullptr == default_value) {
        default_value = std::shared_ptr<RuntimeConfiguration>(new RuntimeConfiguration(builder.get_id()));
    } else {
        default_value->set_entity_id(builder.get_id());
    }
    result = default_value.get();
    builder.add_component(std::shared_ptr(default_value));
    w->create_entity(std::move(builder));
    return *result;
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get(render::engine::Engine* const e)
{
    if (nullptr == e) {
        if (nullptr == default_value) {
            default_value = std::unique_ptr<RuntimeConfiguration>(new RuntimeConfiguration(core::ecs::INVALID_ENTITY_ID));
        }
        return *default_value;
    }

    return get(e->get_world());
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get(Application* const a)
{
    return get(a->get_base().get_render_engine());
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get(Application& a)
{
    return get(a.get_base().get_render_engine());
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get(BaseApplication* const b)
{
    return get(b->get_render_engine());
}

gearoenix::platform::RuntimeConfiguration::~RuntimeConfiguration() = default;

void gearoenix::platform::RuntimeConfiguration::show_debug_gui(const render::engine::Engine& e)
{
    if (!ImGui::TreeNode(core::String::ptr_name(this).c_str())) {
        return;
    }

    Component::show_debug_gui(e);

    if (!ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody)) {
        return;
    }
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 0.999f);
    ImGui::TableNextColumn();

    ImGui::Text("Application Name:");
    ImGui::TableNextColumn();
    ImGui::InputText("##" GX_STRINGIFY(application_name), &application_name);
    ImGui::TableNextColumn();

    ImGui::Text("Fullscreen:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(fullscreen), &fullscreen);
    ImGui::TableNextColumn();

    ImGui::Text("Landscape:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(lanscape), &landscape);
    ImGui::TableNextColumn();

    ImGui::Text("Window Size:");
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Width:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    GX_IMGUI_VAR_INPUT_UINT(window_width);
    ImGui::SameLine();
    ImGui::Text("Height:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    GX_IMGUI_VAR_INPUT_UINT(window_height);
    ImGui::TableNextColumn();

    ImGui::Text("Window Resizing Event Interval MS:");
    ImGui::TableNextColumn();
    GX_IMGUI_VAR_INPUT_UINT(window_resizing_event_interval_ms);
    ImGui::TableNextColumn();

    ImGui::Text("Show Cursor:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(show_cursor), &show_cursor);
    ImGui::TableNextColumn();

    ImGui::Text("Vulkan Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(vulkan_render_backend_enabled), &vulkan_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Direct3DX Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(direct3dx_render_backend_enabled), &direct3dx_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("Metal Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(metal_render_backend_enabled), &metal_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::Text("OpenGL Render Backend Enabled:");
    ImGui::TableNextColumn();
    ImGui::Checkbox("##" GX_STRINGIFY(opengl_render_backend_enabled), &opengl_render_backend_enabled);
    ImGui::TableNextColumn();

    ImGui::EndTable();

    ImGui::TreePop();
}