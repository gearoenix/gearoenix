#include "gx-rnd-mdl-model.hpp"
#include "../imgui/gx-rnd-imgui-type-table.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../material/gx-rnd-mat-material.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"

gearoenix::render::model::Model::Model(
    const core::object_type_index_t final_component_type, const bool is_transformable, meshes_set_t&& meshes, std::string&& name)
    : Component(final_component_type, std::move(name))
    , is_transformable(is_transformable)
    , meshes(std::move(meshes))
{
}

gearoenix::render::model::Model::~Model() = default;

void gearoenix::render::model::Model::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        Component::show_debug_gui();

        imgui::table_scope("##gearoenix::render::model::Model", [this] {
            ImGui::Text("Transformable:");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##is_transformable", &is_transformable);
        });

        if (!meshes.empty() && ImGui::TreeNode("Meshes")) {
            for (const auto& msh : meshes) {
                msh->show_debug_gui();
            }
            ImGui::TreePop();
        }
    });
}

bool gearoenix::render::model::Model::has_transparent_material() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_transparency() == material::Transparency::Transparent) {
            return true;
        }
    }
    return false;
}

bool gearoenix::render::model::Model::needs_mvp() const
{
    for (const auto& msh : meshes) {
        if (msh->get_bound_material()->get_need_model_view_projection_matrix()) {
            return true;
        }
    }
    return false;
}