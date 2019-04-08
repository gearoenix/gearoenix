#ifndef GEAROEMIX_RENDER_TEXTURE_TARGET_HPP
#define GEAROEMIX_RENDER_TEXTURE_TARGET_HPP
#include "rnd-txt-texture-2d.hpp"
#include <memory>
namespace gearoenix {
namespace render {
    namespace command {
        class Buffer;
    }
    namespace texture {
        class Target : public Texture2D {
        protected:
            Target(const core::Id my_id, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Target();
        };
    }
}
}
#endif
