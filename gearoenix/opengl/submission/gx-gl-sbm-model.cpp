#include "gx-gl-sbm-model.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/animation/gx-phs-anm-armature.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/collider/gx-phs-cld-aabb.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../../render/material/gx-rnd-mat-material.hpp"
#include "../gx-gl-camera.hpp"
#include "../gx-gl-engine.hpp"
#include "../gx-gl-mesh.hpp"
#include "../gx-gl-model.hpp"
#include "../material/gx-gl-material.hpp"
#include "gx-gl-sbm-scene.hpp"

gearoenix::gl::submission::Model::Model() = default;

gearoenix::gl::submission::Model::Model(
    const gl::Model* const model,
    const physics::Transformation* const model_transform,
    Scene& scene,
    const physics::animation::Armature* const armature)
    : m(model_transform->get_global_matrix())
    , inv_m(model_transform->get_inverted_global_matrix().transposed())
    , irradiance(scene.default_reflection.second.irradiance)
    , radiance(scene.default_reflection.second.radiance)
    , radiance_lod_coefficient(scene.default_reflection.second.radiance_mips_count)
    , reflection_probe_size(scene.default_reflection.second.size)
    , name(model->get_name().c_str())
{
    const auto [fmi, lmi] = scene.add_meshes(model->get_gl_meshes());
    first_mesh_index = fmi;
    last_mesh_index = lmi;
    if (nullptr != armature) {
        first_bone_index = static_cast<decltype(first_bone_index)>(scene.bones_data.size());
        bones_count = static_cast<decltype(bones_count)>(armature->get_all_bones().size());
        for (const auto* const bone : armature->get_all_bones()) {
            scene.bones_data.push_back(Bone {
                .m = math::Mat4x4<float>(bone->get_global_matrix()),
                .inv_m = math::Mat4x4<float>(bone->get_inverted_global_matrix()),
            });
        }
    }
}

bool gearoenix::gl::submission::Model::has_transparent_material(const Scene& scene) const
{
    for (std::uint32_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        if (scene.meshes[mesh_index].render_material->get_transparency() == render::material::Transparency::Transparent) {
            return true;
        }
    }
    return false;
}

bool gearoenix::gl::submission::Model::needs_mvp(const Scene& scene) const
{
    for (std::uint32_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        if (scene.meshes[mesh_index].material->needs_mvp) {
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
    for (std::uint32_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->shadow(*this, mesh, camera, current_shader);
    }
}

void gearoenix::gl::submission::Model::render_forward(
    const Scene& scene, const Camera& camera, uint& current_shader) const
{
    for (std::uint32_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->forward_render(*this, mesh, camera, scene, current_shader);
    }
}

void gearoenix::gl::submission::Model::render_deferred_gbuffers(
    const Scene& scene, const Camera& camera, uint& current_shader) const
{
    for (std::uint32_t mesh_index = first_mesh_index; mesh_index < last_mesh_index; ++mesh_index) {
        const auto& mesh = scene.meshes[mesh_index];
        mesh.material->deferred_gbuffer_render(*this, mesh, camera, scene, current_shader);
    }
}

gearoenix::gl::submission::BvhNodeModel::BvhNodeModel() = default;

gearoenix::gl::submission::BvhNodeModel::BvhNodeModel(
    const gl::Model* const gl_model,
    const physics::Transformation* const model_transform,
    Scene& scene,
    const physics::animation::Armature* const armature)
    : cameras_flags(gl_model->cameras_flags)
    , model(gl_model, model_transform, scene, armature)
{
}

gearoenix::gl::submission::DynamicModel::DynamicModel(
    const gl::Model* const gl_model,
    const physics::Transformation* const model_transform,
    Scene& scene,
    const physics::animation::Armature* const armature,
    const physics::collider::Aabb3* const collider)
    : base(gl_model, model_transform, scene, armature)
    , box(collider->get_surrounding_box())
{
}

gearoenix::gl::submission::DebugModel::DebugModel(
    const gl::Camera* const camera,
    const physics::Transformation* const transform)
    : m(math::Mat4x4<float>(transform->get_global_matrix()))
    , colour(camera->get_debug_colour())
{
    const auto* const mesh = dynamic_cast<const gl::Mesh*>(camera->get_debug_mesh().get()); // todo: gl camera can hold a casted object
    vertex_object = mesh->get_cached_vertex_object();
    indices_count = mesh->get_cached_indices_count();
}

#endif
