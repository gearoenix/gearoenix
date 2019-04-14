#ifndef GEAROENIX_RENDER_BUFFER_DYNAMIC_HPP
#define GEAROENIX_RENDER_BUFFER_DYNAMIC_HPP
#include "rnd-buf-buffer.hpp"
namespace gearoenix {
namespace render {
    namespace buffer {
        class Dynamic : public Buffer {
        protected:
            Dynamic(const unsigned int s, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Dynamic();
        };
    }
}
}
#endif
