#ifndef GEAROENIX_RENDER_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_SKYBOX_EQUIRECTANGULAR_RESOURCE_SET_HPP
#include "rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class SkyboxEquirectangular;
class SkyboxEquirectangularResourceSet : public ResourceSet {
protected:
    /// It is not owner of any of these pointers
    const buffer::Uniform* material_uniform_buffer = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;

    const mesh::Mesh* msh = nullptr;

    const texture::Texture2D* color = nullptr;

    explicit SkyboxEquirectangularResourceSet(std::shared_ptr<SkyboxEquirectangular> pip) noexcept;

public:
    ~SkyboxEquirectangularResourceSet() noexcept override;
    void set_material(const material::Material* m) noexcept;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
    void clean() noexcept override;
};
}
#endif
