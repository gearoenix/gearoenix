#include "phs-engine.hpp"
#include "../core/cr-semaphore.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include "phs-kernel.hpp"

#ifdef THREAD_SUPPORTED
gearoenix::physics::Engine::Engine(render::Engine* rndeng)
    : render_engine(rndeng)
    , signaller(new core::Semaphore())
{
    // because of some compiler &/| std problems it is here instead of initializer list
    const_cast<unsigned int&>(threads_count) = std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() : 4;
    kernels = new Kernel*[threads_count];

    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i] = new Kernel(i, this);
    }
    std::this_thread::yield();
}
#else
gearoenix::physics::Engine::Engine(render::Engine* rndeng)
    : render_engine(rndeng)
    , kernel(new Kernel(this))
{
}
#endif

gearoenix::physics::Engine::~Engine()
{
#ifdef THREAD_SUPPORTED
    for (unsigned int i = 0; i < threads_count; ++i) {
        delete kernels[i];
    }
    delete[] kernels;
    delete signaller;
#else
    delete kernel;
#endif
}

void gearoenix::physics::Engine::add_animation(std::shared_ptr<animation::Animation> a)
{
    std::lock_guard<std::mutex> lg(pending_animations_locker);
    pending_animations.push_back(a);
}

void gearoenix::physics::Engine::update()
{
#ifdef THREAD_SUPPORTED
    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i]->signal();
    }
#endif
}

void gearoenix::physics::Engine::wait()
{
#ifdef THREAD_SUPPORTED
    for (unsigned int i = 0; i < threads_count; ++i) {
        signaller->lock();
    }
//    std::cout << "physics update ended.\n";
#else
    kernel->signal();
#endif
    if (animations_need_cleaning) {
        animations_need_cleaning = false;
        for (std::map<core::Id, std::shared_ptr<animation::Animation>>::iterator iter = animations.begin(); iter != animations.end();) {
            if (iter->second->is_ended()) {
                iter = animations.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    {
        std::lock_guard<std::mutex> lg(pending_animations_locker);
        for (const std::shared_ptr<animation::Animation>& a : pending_animations) {
#ifdef DEBUG_MODE
            if (animations.find(a->get_id()) != animations.end())
                UNEXPECTED;
#endif
            animations[a->get_id()] = a;
        }
        pending_animations.clear();
    }
}
