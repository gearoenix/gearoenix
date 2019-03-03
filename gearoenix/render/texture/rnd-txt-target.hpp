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
            Target(core::Id my_id, Engine* e);

        public:
            virtual ~Target();
			virtual void bind(const std::shared_ptr<command::Buffer> &c) = 0;
        };
    }
}
}
#endif
