#ifndef GEAROENIX_RENDER_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_SHADOW_MAPPER_RESOURCE_SET_HPP
#include "rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class ShadowMapper;
class ShadowMapperResourceSet : public ResourceSet {
protected:
    const buffer::Uniform* node_uniform_buffer = nullptr;
    const mesh::Mesh* msh = nullptr;
    const texture::Texture2D* color = nullptr;

    explicit ShadowMapperResourceSet(std::shared_ptr<ShadowMapper const> pip) noexcept;

public:
    ~ShadowMapperResourceSet() noexcept override;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_material(const material::Material* m) noexcept;
    void set_node_uniform_buffer(const buffer::Uniform* node_uniform_buffer) noexcept;
    void clean() noexcept override;
};
}
#endif
