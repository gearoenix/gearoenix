#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
namespace gearoenix {
namespace physics {
    class Kernel;
    class Engine {
    private:
        const unsigned int number_of_threads;
        Kernel** kernels;

    protected:
    public:
        Engine();
        ~Engine();
        void update();
    };
}
}
#endif
