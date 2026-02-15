#include "gx-rnd-msh-mesh.hpp"
#include "../imgui/gx-rnd-imgui-type-tree.hpp"
#include "../material/gx-rnd-mat-material.hpp"

gearoenix::render::mesh::Mesh::Mesh(std::string&& name, std::shared_ptr<Buffer>&& buffer, std::shared_ptr<material::Material>&& material)
    : Object(gearoenix_render_mesh_type_index, std::move(name))
    , buffer(std::move(buffer))
    , bound_material(std::move(material))
{
}

gearoenix::render::mesh::Mesh::~Mesh() = default;

void gearoenix::render::mesh::Mesh::set_material(std::shared_ptr<material::Material>&& material) { bound_material = std::move(material); }

void gearoenix::render::mesh::Mesh::show_debug_gui()
{
    imgui::tree_scope(this, [this] {
        if (ImGui::TreeNode("Buffer")) {

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Bound Material")) {
            bound_material->show_debug_gui();
            ImGui::TreePop();
        }
    });
}
