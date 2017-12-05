#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
namespace gearoenix {
namespace core {
    class Semaphore;
}
namespace render {
    class Engine;
}
namespace physics {
    class Kernel;
    class Engine {
        friend class Kernel;

    private:
        render::Engine* render_engine;
#ifdef THREAD_SUPPORTED
        const unsigned int threads_count = 4;
        core::Semaphore* signaller;
        Kernel** kernels;
#else
        Kernel* kernel;
#endif

    protected:
    public:
        Engine(render::Engine* rndeng);
        ~Engine();
        void update();
        void wait();
    };
}
}
#endif
