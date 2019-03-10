#ifndef GEAROEMIX_RENDER_TEXTURE_MANAGER_HPP
#define GEAROEMIX_RENDER_TEXTURE_MANAGER_HPP
#include <memory>
#include "../../core/cache/cr-cache-file.hpp"

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
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
			core::cache::File<Texture> cache;

        public:
            Manager(const std::shared_ptr<system::stream::Stream>& s, const std::shared_ptr<engine::Engine>& e);
            ~Manager();
        };
    }
}
}
#endif
