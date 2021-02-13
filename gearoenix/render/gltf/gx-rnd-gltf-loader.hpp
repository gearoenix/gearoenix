#ifndef GEAROENIX_RENDER_GLTF_LOADER_HPP
#define GEAROENIX_RENDER_GLTF_LOADER_HPP

#include <memory>
#include <string>

namespace tinygltf {
class Model;
class TinyGLTF;
}

namespace gearoenix::render::gltf {
struct Loader final {
    std::unique_ptr<tinygltf::TinyGLTF> context;
    std::unique_ptr<tinygltf::Model> data;

    static void check(bool, const std::string& err, const std::string& warn) noexcept;

    explicit Loader(const std::string& file_address) noexcept;
    ~Loader() noexcept;
};
}

#endif