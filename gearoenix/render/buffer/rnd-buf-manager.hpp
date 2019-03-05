#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
namespace gearoenix {
namespace render {
    class Engine;
    namespace buffer {
        class Uniform;
        class Manager {
        protected:
        public:
            Manager(Engine*) {}
            ~Manager() {}

            virtual Uniform* create_uniform(unsigned int size);
        };
    } // namespace buffer
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_BUFFER_MANAGER_HPP
