#ifndef GEAROENIX_RENDER_PIPELINE_SKY_CUBE_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_SKY_CUBE_RESOURCE_SET_HPP
#include "../../core/cr-build-configuration.hpp"
#include "rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class SkyCubeResourceSet : public ResourceSet {
protected:
    /// It is not owner of any of these pointers
    const buffer::Uniform* material_uniform_buffer = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;

    const mesh::Mesh* msh = nullptr;

    const texture::TextureCube* color = nullptr;

public:
    ~SkyCubeResourceSet() noexcept override = default;
    void set_material(const material::Material* m) noexcept;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
    void clean() noexcept override;
};
}
#endif
