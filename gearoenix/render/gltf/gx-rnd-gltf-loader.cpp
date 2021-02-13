#include "gx-rnd-gltf-loader.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-rnd-gltf-include.hpp"

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

gearoenix::render::gltf::Loader::Loader(const std::string& file_address) noexcept
    : context(new tinygltf::TinyGLTF)
    , data(new tinygltf::Model)
{
    bool result;
    std::string err, warn;
    if (file_address.ends_with(".glb"))
        result = context->LoadBinaryFromFile(data.get(), &err, &warn, file_address);
    else if (file_address.ends_with(".gltf"))
        result = context->LoadASCIIFromFile(data.get(), &err, &warn, file_address);
    else
        GX_LOG_F("Unsupported/unexpected extension for file: " << file_address)
    check(result, err, warn);
}

gearoenix::render::gltf::Loader::~Loader() noexcept = default;
