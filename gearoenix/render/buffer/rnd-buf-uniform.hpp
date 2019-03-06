#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#include <memory>
namespace gearoenix {
namespace render {
	namespace engine {
		class Engine;
	}
    namespace buffer {
        class Uniform {
        protected:
            const std::shared_ptr<engine::Engine> e;
        public:
            Uniform(const std::shared_ptr<engine::Engine> &e);
            virtual ~Uniform();
            virtual void update(const void*, unsigned int) = 0;
            virtual void* get_data() = 0;
            
			template <typename T>
            void update(const T& s)
            {
                /// todo in debug mode check for the size of uniform buffer
                update(s, sizeof(T));
            }
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
