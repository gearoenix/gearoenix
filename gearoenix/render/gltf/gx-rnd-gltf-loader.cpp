#include "gx-rnd-gltf-loader.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "gx-rnd-gltf-mesh-manager.hpp"
#include "gx-rnd-gltf-node-manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

void gearoenix::render::gltf::Loader::check(
    const bool result, const std::string& err, const std::string& warn) noexcept
{
    if (!warn.empty())
        GX_LOG_E("Warning in GLTF loader: " << warn)
    if (!err.empty())
        GX_LOG_F("Error in GLTF loader: " << err)
    if (!result)
        GX_LOG_F("Error in GLTF loader.")
}

gearoenix::render::gltf::Loader::Loader(engine::Engine* e, const platform::stream::Path& file) noexcept
    : e(e)
    , context(new tinygltf::TinyGLTF)
    , data(new tinygltf::Model)
    , mesh_manager(new MeshManager(*this))
    , node_manager(new NodeManager(*this, *mesh_manager))
{
    bool result;
    std::string err, warn;
    /// TODO better file import
    const auto& file_address = file.get_raw_data();
    if (file_address.ends_with(".glb"))
        result = context->LoadBinaryFromFile(data.get(), &err, &warn, file_address);
    else if (file_address.ends_with(".gltf"))
        result = context->LoadASCIIFromFile(data.get(), &err, &warn, file_address);
    else
        GX_LOG_F("Unsupported/unexpected extension for file: " << file_address)
    check(result, err, warn);
}

gearoenix::render::gltf::Loader::~Loader() noexcept = default;
