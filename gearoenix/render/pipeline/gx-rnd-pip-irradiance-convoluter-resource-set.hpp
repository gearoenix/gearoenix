#ifndef GEAROENIX_RENDER_PIPELINE_IRRADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_IRRADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#include "gx-rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class IrradianceConvoluter;
class IrradianceConvoluterResourceSet : public ResourceSet {
protected:
    const mesh::Mesh* msh = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;
    const texture::TextureCube* environment = nullptr;

    explicit IrradianceConvoluterResourceSet(std::shared_ptr<IrradianceConvoluter const> pip) noexcept;

public:
    ~IrradianceConvoluterResourceSet() noexcept override;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_environment(const texture::TextureCube* environment) noexcept;
    void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
    void clean() noexcept override;
};
}
#endif
