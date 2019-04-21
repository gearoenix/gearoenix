#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#include "rnd-buf-buffer.hpp"
namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform : public Buffer {
        protected:
            Uniform(const unsigned int s, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Uniform();
            virtual void update(const void*) = 0;
            virtual void* get_data() = 0;
        };
    }
}
}
#endif