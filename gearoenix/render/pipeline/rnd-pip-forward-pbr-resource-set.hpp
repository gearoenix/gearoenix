#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#include "rnd-pip-resource-set.hpp"
#include "../../core/cr-build-configuration.hpp"
namespace gearoenix::render::pipeline {
class ForwardPbrResourceSet : public ResourceSet {
protected:
    texture::Cube* diffuse_environment = nullptr;
    texture::Cube* specular_environment = nullptr;
    texture::Texture2D* ambient_occlusion = nullptr;
    texture::Texture2D* directional_lights_shadow_maps[GX_MAX_DIRECTIONAL_LIGHTS_SHADOW_CASTER][GX_MAX_SHADOW_CASCADES] = {};
    texture::Texture2D* brdflut = nullptr;

public:
    virtual void set_node_uniform_buffer(buffer::Uniform* uniform_buffer) noexcept;
    virtual void set_diffuse_environment(texture::Cube* t) noexcept;
    virtual void set_specular_environment(texture::Cube* t) noexcept;
    virtual void set_ambient_occlusion(texture::Texture2D* t) noexcept;
    virtual void set_directional_lights_shadow_maps(texture::Texture2D* t, int light_index, int cas_index) noexcept;
    virtual void set_brdflut(texture::Texture2D* t) noexcept;
    virtual void clean() noexcept;
};
}
#endif
