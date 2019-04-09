#ifndef GEAROEMIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#define GEAROEMIX_RENDER_TEXTURE_TEXTURE_2D_HPP
#include "rnd-txt-texture.hpp"
namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
    namespace texture {
        class Texture2D : public Texture {
        protected:
			unsigned int img_width = 0;
			unsigned int img_height = 0;
            Texture2D(const core::Id my_id, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Texture2D();
        };
    }
}
}
#endif
