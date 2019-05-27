#ifndef GEAROENIX_RENDER_BUFFER_DYNAMIC_HPP
#define GEAROENIX_RENDER_BUFFER_DYNAMIC_HPP
#include "rnd-buf-buffer.hpp"
namespace gearoenix::render::buffer {
        class Dynamic : public Buffer {
        protected:
            Dynamic(const unsigned int s, engine::Engine* const e) noexcept : Buffer(s, e) {}

        public:
            virtual ~Dynamic() noexcept = default;
        };
    }
#endif
