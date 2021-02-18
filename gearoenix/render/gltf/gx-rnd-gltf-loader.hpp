#ifndef GEAROENIX_RENDER_GLTF_LOADER_HPP
#define GEAROENIX_RENDER_GLTF_LOADER_HPP
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <string>

namespace gearoenix::platform::stream {
struct Path;
}

namespace tinygltf {
class Model;
class TinyGLTF;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::gltf {
struct MeshManager;
struct NodeManager;
struct Loader final {
    GX_GET_PTRC_PRV(engine::Engine, e)
    GX_GET_UPTR_PRV(tinygltf::TinyGLTF, context)
    GX_GET_UPTR_PRV(tinygltf::Model, data)
    GX_GET_UPTR_PRV(MeshManager, mesh_manager)
    GX_GET_UPTR_PRV(NodeManager, node_manager)

    static void check(bool, const std::string& err, const std::string& warn) noexcept;

    explicit Loader(engine::Engine* e, const platform::stream::Path& file) noexcept;
    ~Loader() noexcept;
};
}

#endif