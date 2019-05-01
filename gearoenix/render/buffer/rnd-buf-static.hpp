#ifndef GEAROENIX_RENDER_BUFFER_STATIC_HPP
#define GEAROENIX_RENDER_BUFFER_STATIC_HPP
#include "rnd-buf-buffer.hpp"

namespace gearoenix {
namespace render {
    namespace buffer {
        class Static : public Buffer {
        protected:
            Static(const unsigned int size, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Static();
        };
    }
}
}
#endif
