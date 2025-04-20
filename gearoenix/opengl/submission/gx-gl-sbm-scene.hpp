#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../gx-gl-model.hpp"
#include "gx-gl-sbm-animation.hpp"
#include "gx-gl-sbm-environment.hpp"
#include "gx-gl-sbm-light.hpp"
#include "gx-gl-sbm-mesh.hpp"
#include "gx-gl-sbm-model.hpp"
#include <boost/container/flat_map.hpp>

namespace gearoenix::core::ecs {
struct Entity;
}

namespace gearoenix::gl {
struct Mesh;
}

namespace gearoenix::gl::submission {
struct Scene final {
    boost::container::flat_map<std::tuple<double /*layer*/, core::ecs::Entity*, bool /*equirectangular*/>, Skybox> skyboxes;

    [[nodiscard]] std::pair<std::uint32_t, std::uint32_t> add_meshes(const gl::Model::gl_meshes_set_t& gl_meshes);
};
}

#endif