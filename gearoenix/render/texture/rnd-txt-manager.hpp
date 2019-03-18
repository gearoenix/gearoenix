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
		class Texture;
		class Texture2D;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Texture> cache;
			std::map<math::Vec4, core::Id> color_id;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();

			std::shared_ptr<Texture2D> get(const math::Vec4 &color, core::sync::EndCaller<Texture2D> &c, const unsigned int color_count = 4);
			std::shared_ptr<Texture2D> get(const math::Vec3 &color, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Texture2D> get(const math::Vec2 &color, core::sync::EndCaller<Texture2D> &c);
			std::shared_ptr<Texture> get_gx3d(const core::Id id, core::sync::EndCaller<Texture> &c);
        };
    }
}
}
#endif
