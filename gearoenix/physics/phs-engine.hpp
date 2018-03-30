#ifndef GEAROENIX_PHYSICS_ENGINE_HPP
#define GEAROENIX_PHYSICS_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#include "../core/cr-types.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <vector>
namespace gearoenix {
namespace core {
    namespace sync {
        class Semaphore;
		class StopPoint;
    }
}
namespace render {
    class Engine;
}
namespace physics {
    class Kernel;
    namespace animation {
        class Animation;
    }
    class Engine {
        friend class Kernel;

    private:
        render::Engine* render_engine;
        // owner
        // if animation return true on its apply its gonna be deleted
        std::map<core::Id, std::shared_ptr<animation::Animation>> animations;
        std::mutex pending_animations_locker;
        std::vector<std::shared_ptr<animation::Animation>> pending_animations;
        bool animations_need_cleaning = false;
        const unsigned int threads_count = 4;
        core::sync::Semaphore* signaller;
        Kernel** kernels;
		core::sync::StopPoint* kernels_piont = nullptr;

    protected:
    public:
        Engine(render::Engine* rndeng);
        ~Engine();
        // engine gonna remove it from its active animations, caller must take care of its deleteing
        void add_animation(std::shared_ptr<animation::Animation> a);
        // caller must take care of deleting
        void remove_animation(std::shared_ptr<animation::Animation> a);
        void update();
        void wait();
    };
}
}
#endif
