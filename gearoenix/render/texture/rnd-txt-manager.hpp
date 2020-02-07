#ifndef GEAROENIX_RENDER_TEXTURE_MANAGER_HPP
#define GEAROENIX_RENDER_TEXTURE_MANAGER_HPP
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-txt-info.hpp"
#include <map>
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::texture {
class Cube;
class Texture;
class Texture2D;
class Manager {
protected:
    engine::Engine* const e;
    core::cache::File<Texture> cache;
    std::map<math::Vec4, core::Id> color_4d_id_t2d;
    std::map<math::Vec3, core::Id> color_3d_id_t2d;
    std::map<math::Vec2, core::Id> color_2d_id_t2d;
    std::map<core::Real, core::Id> color_1d_id_t2d;
    std::map<math::Vec4, core::Id> color_4d_id_cube;
    std::map<math::Vec3, core::Id> color_3d_id_cube;
    std::map<math::Vec2, core::Id> color_2d_id_cube;
    std::shared_ptr<Texture2D> default_one_1c_2d;
    std::shared_ptr<Texture2D> default_one_2c_2d;
    std::shared_ptr<Cube> default_zero_3c_cube;

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    std::shared_ptr<Texture2D> get_2d(const math::Vec4& color, core::sync::EndCaller<Texture2D>& c) noexcept;
    std::shared_ptr<Texture2D> get_2d(const math::Vec3& color, core::sync::EndCaller<Texture2D>& c) noexcept;
    std::shared_ptr<Texture2D> get_2d(const math::Vec2& color, core::sync::EndCaller<Texture2D>& c) noexcept;
    std::shared_ptr<Texture2D> get_2d(core::Real value, core::sync::EndCaller<Texture2D>& c) noexcept;
    std::shared_ptr<Texture2D> get_2d_one_1c(core::sync::EndCaller<Texture2D>& c) noexcept;
    std::shared_ptr<Texture2D> get_2d_one_2c(core::sync::EndCaller<Texture2D>& c) noexcept;
    /// It will take ownership of data
    std::shared_ptr<Texture2D> create_2d(unsigned char* data, const Info& info, int img_width, int img_height, core::sync::EndCaller<Texture2D>& c) noexcept;
    /// It creates 2d floating texture from formatted data
    std::shared_ptr<Texture2D> create_2d_f(const unsigned char* data, std::size_t size, core::sync::EndCaller<Texture2D>& c, const SampleInfo& sample_info = SampleInfo()) noexcept;
    /// It creates 2d floating texture from file
    std::shared_ptr<Texture2D> create_2d_f(const std::wstring& file_address, core::sync::EndCaller<Texture2D>& c, const SampleInfo& sample_info = SampleInfo()) noexcept;
    std::shared_ptr<Cube> get_cube(const math::Vec4& color, core::sync::EndCaller<Cube>& c) noexcept;
    std::shared_ptr<Cube> get_cube(const math::Vec3& color, core::sync::EndCaller<Cube>& c) noexcept;
    std::shared_ptr<Cube> get_cube(const math::Vec2& color, core::sync::EndCaller<Cube>& c) noexcept;
    std::shared_ptr<Cube> get_cube_zero_3c(core::sync::EndCaller<Cube>& c) noexcept;
    std::shared_ptr<Texture> get_gx3d(core::Id id, core::sync::EndCaller<Texture>& c) noexcept;
    [[nodiscard]] engine::Engine* get_engine() const noexcept;
};
}
#endif
