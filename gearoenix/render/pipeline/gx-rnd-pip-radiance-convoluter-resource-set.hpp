#ifndef GEAROENIX_RENDER_PIPELINE_RADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_RADIANCE_CONVOLUTER_RESOURCE_SET_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#include "gx-rnd-pip-resource-set.hpp"

namespace gearoenix::render::pipeline {
class RadianceConvoluter;
class RadianceConvoluterResourceSet : public ResourceSet {
protected:
    const mesh::Mesh* msh = nullptr;
    const buffer::Uniform* node_uniform_buffer = nullptr;
    const texture::TextureCube* environment = nullptr;

    explicit RadianceConvoluterResourceSet(std::shared_ptr<RadianceConvoluter const> pip) noexcept;

public:
    ~RadianceConvoluterResourceSet() noexcept override;
    void set_mesh(const mesh::Mesh* m) noexcept;
    void set_environment(const texture::TextureCube* environment) noexcept;
    void set_node_uniform_buffer(const buffer::Uniform* u) noexcept;
    void clean() noexcept override;
};
}
#endif
