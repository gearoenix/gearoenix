#ifndef GEAROENIX_OPENGL_BUFFER_UNIFORM_HPP
#define GEAROENIX_OPENGL_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace buffer {
        class SubBuffer;
        class Uniform {
        private:
        public:
            Uniform(unsigned int, Engine*);
            ~Uniform();
            const SubBuffer* const* get_vbuf() const;
            const SubBuffer* const* get_cbuf() const;
            unsigned int get_count() const;
            unsigned int get_size() const;
            void update(const void*, unsigned int);
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif
#endif // GEAROENIX_OPENGL_BUFFER_UNIFORM_HPP
