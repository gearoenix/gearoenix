#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace gearoenix::render::texture {
struct Texture2D;
}

namespace gearoenix::render::gltf {
struct Context;
struct Textures final {
    const Context& context;
    std::vector<std::string> textures_names;
    std::mutex textures_map_lock;
    std::map<std::string, std::shared_ptr<texture::Texture2D>> textures_map;
    /// Maps (mr_tex_index, occ_tex_index) to the name of the merged ORM texture in textures_map
    std::map<std::pair<int, int>, std::string> orm_names;

    explicit Textures(const Context&);
    ~Textures();
    void load(int index, core::job::EndCaller<>&&);
    void load_orm(int mr_tex_index, int occ_tex_index, core::job::EndCaller<>&&);
    void load(core::job::EndCaller<>&&);
    [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get(int index, const std::shared_ptr<texture::Texture2D>& default_value) const;
    [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_orm(int mr_tex_index, int occ_tex_index, const std::shared_ptr<texture::Texture2D>& default_value) const;
};
}