#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
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
        const unsigned int threads_count = 4;
        render::Engine* render_engine;
        core::Semaphore* signaller;
        Kernel** kernels;

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
