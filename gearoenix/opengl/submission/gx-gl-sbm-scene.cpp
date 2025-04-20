#include "gx-gl-sbm-scene.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED

std::pair<std::uint32_t, std::uint32_t> gearoenix::gl::submission::Scene::add_meshes(
    const gl::Model::gl_meshes_set_t& gl_meshes)
{
    const auto first_mesh_index = static_cast<std::uint32_t>(meshes.size());
    for (const auto& mesh : gl_meshes) {
        meshes.emplace_back(*mesh);
    }
    const auto last_mesh_index = static_cast<std::uint32_t>(meshes.size());
    return { first_mesh_index, last_mesh_index };
}

#endif
