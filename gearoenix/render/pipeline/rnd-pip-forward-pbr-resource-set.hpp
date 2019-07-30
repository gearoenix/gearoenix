#ifndef GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_FORWARD_PBR_RESOURCE_SET_HPP
#include "rnd-pip-resource-set.hpp"
namespace gearoenix::render::pipeline {
class ForwardPbrResourceSet : public ResourceSet {
protected:
    texture::Cube* diffuse_environment = nullptr;
    texture::Cube* specular_environment = nullptr;
    texture::Texture2D* ambient_occlusion = nullptr;
    texture::Texture2D* shadow_map = nullptr;
    texture::Texture2D* brdflut = nullptr;

public:
    virtual void set_node_uniform_buffer(buffer::Uniform* uniform_buffer) noexcept;
    virtual void set_diffuse_environment(texture::Cube* t) noexcept;
    virtual void set_specular_environment(texture::Cube* t) noexcept;
    virtual void set_ambient_occlusion(texture::Texture2D* t) noexcept;
    virtual void set_shadow_mapper(texture::Texture2D* t) noexcept;
    virtual void set_brdflut(texture::Texture2D* t) noexcept;
    virtual void clean() noexcept;
};
}
#endif
