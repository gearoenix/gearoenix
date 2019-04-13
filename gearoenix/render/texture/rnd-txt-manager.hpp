#ifndef GEAROEMIX_RENDER_TEXTURE_MANAGER_HPP
#define GEAROEMIX_RENDER_TEXTURE_MANAGER_HPP
#include <memory>
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../math/math-vector.hpp"
#include <map>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace texture {
		class Cube;
		class Texture;
		class Texture2D;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Texture> cache;
			std::map<math::Vec4, core::Id> color_4d_id_t2d;
			std::map<math::Vec3, core::Id> color_3d_id_t2d;
			std::map<math::Vec2, core::Id> color_2d_id_t2d;
			std::map<core::Real, core::Id> color_1d_id_t2d;
			std::map<math::Vec4, core::Id> color_4d_id_cube;
			std::map<math::Vec3, core::Id> color_3d_id_cube;
			std::map<math::Vec2, core::Id> color_2d_id_cube;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();

			std::shared_ptr<Texture2D> get_2d(const math::Vec4 &color, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Texture2D> get_2d(const math::Vec3 &color, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Texture2D> get_2d(const math::Vec2 &color, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Texture2D> get_2d(const core::Real value, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Cube> get_cube(const math::Vec4 &color, core::sync::EndCaller<Cube> &c);
			std::shared_ptr<Cube> get_cube(const math::Vec3 &color, core::sync::EndCaller<Cube> &c);
			std::shared_ptr<Cube> get_cube(const math::Vec2 &color, core::sync::EndCaller<Cube> &c);
			std::shared_ptr<Texture> get_gx3d(const core::Id id, core::sync::EndCaller<Texture> &c);
        };
    }
}
}
#endif
