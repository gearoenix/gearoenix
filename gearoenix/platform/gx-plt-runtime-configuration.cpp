#include "gx-plt-runtime-configuration.hpp"
#include "../core/gx-cr-build-configuration.hpp"
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../render/imgui/gx-rnd-imgui-input-uint.hpp"
#include "../render/imgui/gx-rnd-imgui-type-table.hpp"
#include "../render/imgui/gx-rnd-imgui-type-tree.hpp"
#include "gx-plt-application.hpp"
#include "stream/gx-plt-stm-stream.hpp"
#include <ImGui/misc/cpp/imgui_stdlib.h>

void gearoenix::platform::RuntimeConfiguration::write(stream::Stream& s) const
{
    s.write_fail_debug(application_name);
    s.write_fail_debug(fullscreen);
    s.write_fail_debug(landscape);
    s.write_fail_debug(window_width);
    s.write_fail_debug(window_height);
    s.write_fail_debug(window_resizing_event_interval_ms);
    s.write_fail_debug(show_cursor);
    s.write_fail_debug(vulkan_render_backend_enabled);
    s.write_fail_debug(direct3dx_render_backend_enabled);
    s.write_fail_debug(metal_render_backend_enabled);
    s.write_fail_debug(opengl_render_backend_enabled);
}

void gearoenix::platform::RuntimeConfiguration::read(stream::Stream& s)
{
    s.read(application_name);
    s.read(fullscreen);
    s.read(landscape);
    s.read(window_width);
    s.read(window_height);
    s.read(window_resizing_event_interval_ms);
    s.read(show_cursor);
    s.read(vulkan_render_backend_enabled);
    s.read(direct3dx_render_backend_enabled);
    s.read(metal_render_backend_enabled);
    s.read(opengl_render_backend_enabled);
}

gearoenix::platform::RuntimeConfiguration::RuntimeConfiguration()
    : application_name(GX_APPLICATION_NAME)
{
}

gearoenix::platform::RuntimeConfiguration& gearoenix::platform::RuntimeConfiguration::get()
{
    static RuntimeConfiguration instance;
    return instance;
}

gearoenix::platform::RuntimeConfiguration::~RuntimeConfiguration() = default;

void gearoenix::platform::RuntimeConfiguration::show_debug_gui()
{
    render::imgui::tree_scope(this, [this] {
        render::imgui::table_scope("##gearoenix::platform::RuntimeConfiguration::show_debug_gui", [this] {
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
        });
    });
}