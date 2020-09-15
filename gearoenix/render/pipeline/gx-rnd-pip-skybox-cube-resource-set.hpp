#ifndef GEAROENIX_RENDER_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_SKYBOX_CUBE_RESOURCE_SET_HPP
#include "gx-rnd-pip-resource-set.hpp"

namespace gearoenix::render::material {
class SkyboxCube;
}

namespace gearoenix::render::pipeline {
class SkyboxCube;
class SkyboxCubeResourceSet : public ResourceSet {
protected:
    /// It is not owner of any of these pointers
    const buffer::Uniform* material_uniform_buffer = nullptr;
    const buffer::Uniform* camera_uniform_buffer = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;

    const mesh::Mesh* msh = nullptr;

    const texture::TextureCube* color = nullptr;

    explicit SkyboxCubeResourceSet(std::shared_ptr<SkyboxCube const> pip) noexcept;

public:
    ~SkyboxCubeResourceSet() noexcept override;
    void set_material(const material::SkyboxCube* m) noexcept;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_camera(const camera::Camera* cam) noexcept;
    void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
    void clean() noexcept override;
};
}
#endif
