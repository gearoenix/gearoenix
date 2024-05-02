#include "gx-gl-sbm-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../render/material/gx-rnd-mat-material.hpp"
#include "../material/gx-gl-material.hpp"
#include "gx-gl-sbm-camera.hpp"
#include "gx-gl-sbm-scene.hpp"

bool gearoenix::gl::submission::Model::has_transparent_material(const Scene& scene) const
{
    for (std::size_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        if (scene.meshes[mesh_index].render_material->get_transparency() == render::material::Transparency::Transparent) {
            return true;
        }
    }
    return false;
}

bool gearoenix::gl::submission::Model::needs_mvp(const gearoenix::gl::submission::Scene& scene) const
{
    for (std::size_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        if (!scene.meshes[mesh_index].material->needs_mvp) {
            return true;
        }
    }
    return false;
}

void gearoenix::gl::submission::Model::render_shadow(
    const Scene& scene,
    const Camera& camera,
    uint& current_shader) const
{
    for (std::size_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->shadow(*this, mesh, camera, current_shader);
    }
}

void gearoenix::gl::submission::Model::render_forward(
    const Scene& scene, const Camera& camera, uint& current_shader) const
{
    for (std::size_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->forward_render(*this, mesh, camera, scene, current_shader);
    }
}

void gearoenix::gl::submission::Model::render_deferred_gbuffers(
    const Scene& scene, const Camera& camera, uint& current_shader) const
{
    for (std::size_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->deferred_gbuffer_render(*this, mesh, camera, scene, current_shader);
    }
}

#endif
