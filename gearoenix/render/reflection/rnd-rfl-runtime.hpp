#ifndef GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#define GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-rfl-reflection.hpp"

namespace gearoenix::render::camera {
class Perspective;
}

namespace gearoenix::render::graph::node {
class IrradianceConvoluter;
class RadianceConvoluter;
class MipmapGenerator;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::texture {
class Target;
}

namespace gearoenix::render::reflection {
class Runtime final : public Reflection {
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube>, environment)
    GX_GET_CREF_PRV(std::shared_ptr<graph::node::MipmapGenerator>, environment_mipmap_generator)
    GX_GET_ARRC_PRV(std::shared_ptr<camera::Perspective>, cameras, 6)
    GX_GET_ARRC_PRV(std::shared_ptr<texture::Target>, environment_targets, 6)
    GX_GET_ARRC_PRV(std::shared_ptr<texture::Target>, diffuse_targets, 6)
    GX_GET_CREF_PRT(std::vector<core::Real>, roughnesses)
    GX_GET_ARRC_PRV(std::vector<std::shared_ptr<texture::Target>>, specular_targets, 6)
    GX_GET_ARRC_PRV(std::shared_ptr<graph::node::IrradianceConvoluter>, irradiance_convoluters, 6)
    GX_GET_ARRC_PRV(std::vector<std::shared_ptr<graph::node::RadianceConvoluter>>, radiance_convoluters, 6)
    GX_GET_ARRC_PRV(std::shared_ptr<mesh::Mesh>, face_meshes, 6)
    GX_GET_VAL_PRV(core::Real, receiving_radius, 20.0f)
public:
    Runtime(core::Id id, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    ~Runtime() noexcept final;

    void set_receiving_radius(core::Real r) noexcept;
};
}
#endif