#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#include <memory>
namespace gearoenix {
namespace render {
    class Engine;
    namespace buffer {
        class Uniform {
        protected:
            Engine* engine;

        public:
            Uniform(Engine*);
            virtual ~Uniform();
			template<typename T> void update(const T& s) {
				/// todo in debug mode check for the size of uniform buffer
				update(s, sizeof(T));
			}
            virtual void update(const void*, unsigned int) = 0;
            virtual void* get_data() = 0;
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
