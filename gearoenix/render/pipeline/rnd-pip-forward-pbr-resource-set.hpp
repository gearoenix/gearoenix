#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP

#include "../../core/cr-build-configuration.hpp"
#include "rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class ForwardPbrResourceSet : public ResourceSet {
protected:
    /// It is not owner of any of these pointers
    const buffer::Uniform* scene_uniform_buffer = nullptr;
    const buffer::Uniform* camera_uniform_buffer = nullptr;
    const buffer::Uniform* model_uniform_buffer = nullptr;
    const buffer::Uniform* material_uniform_buffer = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;

    const mesh::Mesh* msh = nullptr;

    const texture::Texture2D* color = nullptr;
    const texture::Texture2D* metallic_roughness = nullptr;
    const texture::Texture2D* normal = nullptr;
    const texture::Texture2D* emissive = nullptr;
    const texture::Cube* diffuse_environment = nullptr;
    const texture::Cube* specular_environment = nullptr;
    const texture::Texture2D* ambient_occlusion = nullptr;
    const texture::Texture2D* directional_lights_shadow_maps[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER][GX_MAX_SHADOW_CASCADES] = {};
    const texture::Texture2D* brdflut = nullptr;

public:
    ~ForwardPbrResourceSet() noexcept override = default;
#define GX_HELPER(c, cc) void set_##c(const c::cc* o) noexcept
    GX_HELPER(scene, Scene);
    GX_HELPER(camera, Camera);
    GX_HELPER(model, Model);
    GX_HELPER(mesh, Mesh);
    GX_HELPER(material, Material);
#undef GX_HELPER

    void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
    void set_diffuse_environment(texture::Cube* t) noexcept;
    void set_specular_environment(texture::Cube* t) noexcept;
    void set_ambient_occlusion(texture::Texture2D* t) noexcept;
    void set_directional_lights_shadow_map(texture::Texture2D* t, int light_index, int cas_index) noexcept;
    void set_brdflut(texture::Texture2D* t) noexcept;
    void clean() noexcept override;
};
}
#endif
