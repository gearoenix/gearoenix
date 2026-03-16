#include "gx-rnd-mat-material.hpp"
#include "../imgui/gx-rnd-imgui-type-table.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include <utility>

gearoenix::render::material::Material::Material(const core::object_type_index_t final_object_type, std::string&& name, const bool need_model_view_projection_matrix)
    : Object(final_object_type, std::move(name))
    , need_model_view_projection_matrix(need_model_view_projection_matrix)
{
}

gearoenix::render::material::Material::~Material() = default;

void gearoenix::render::material::Material::set_albedo(std::shared_ptr<texture::Texture2D>&&) { GX_UNIMPLEMENTED; }

void gearoenix::render::material::Material::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Object::show_debug_gui();

        imgui::table_scope("##gearoenix::render::material::Material", [this] {
            ImGui::Text("Is Shadow Caster: ");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##is_shadow_caster", &is_shadow_caster);
            ImGui::TableNextColumn();

            ImGui::Text("Is Shadow Receiver: ");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##is_shadow_receiver", &is_shadow_receiver);
            ImGui::TableNextColumn();

            ImGui::Text("Transparency: ");
            ImGui::TableNextColumn();
            material::show_debug_gui(transparency);
            ImGui::TableNextColumn();

            ImGui::Text("Need Model View Projection Matrix: ");
            ImGui::TableNextColumn();
            ImGui::Text("%s", need_model_view_projection_matrix ? "True" : "False");
        });
    });
}
