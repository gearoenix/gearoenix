#ifndef GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#define GEAROENIX_RENDER_BUFFER_BUFFER_HPP
namespace gearoenix::render {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Buffer {
        protected:
            engine::Engine* const e;
            const unsigned int size;

            Buffer(const unsigned int size, engine::Engine*const e) noexcept : e(e), size(size) {}

        public:
            virtual ~Buffer() noexcept = default;
            unsigned int get_size() const noexcept { return size; };
        };
    }
}
#endif
