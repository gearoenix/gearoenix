#include "phs-engine.hpp"
#include "../core/sync/cr-sync-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include "phs-kernel.hpp"

gearoenix::physics::Engine::Engine(render::Engine* rndeng)
    : render_engine(rndeng)
    , signaller(new core::sync::Semaphore())
{
    // because of some compiler &/| std problems it is here instead of initializer list
    const_cast<unsigned int&>(threads_count) = std::thread::hardware_concurrency() > 4 ? std::thread::hardware_concurrency() : 4;
    kernels_piont_animations = new core::sync::StopPoint(threads_count);
    kernels_piont_constraints = new core::sync::StopPoint(threads_count);
    kernels_piont_bodies = new core::sync::StopPoint(threads_count);
    kernels = new Kernel*[threads_count];
    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i] = new Kernel(i, this);
    }
    std::this_thread::yield();
}

gearoenix::physics::Engine::~Engine()
{
    for (unsigned int i = 0; i < threads_count; ++i) {
        delete kernels[i];
    }
    delete[] kernels;
    delete signaller;
    delete kernels_piont_animations;
    delete kernels_piont_constraints;
    delete kernels_piont_bodies;
}

void gearoenix::physics::Engine::add_animation(std::shared_ptr<animation::Animation> a)
{
    std::lock_guard<std::mutex> lg(pending_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a->get_id()) != animations.end())
        GXUNEXPECTED;
#endif
    pending_animations.push_back(a);
}

void gearoenix::physics::Engine::update()
{
    for (unsigned int i = 0; i < threads_count; ++i) {
        kernels[i]->signal();
    }
}

void gearoenix::physics::Engine::wait()
{
    for (unsigned int i = 0; i < threads_count; ++i) {
        signaller->lock();
    }
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
        for (const std::shared_ptr<animation::Animation>& an : pending_animations) {
            {
                const std::map<core::Id, std::shared_ptr<animation::Animation>>::iterator iter = animations.find(an->get_id());
                if (iter != animations.end())
                    if (iter->second->get_id() == an->get_id())
                        GXUNEXPECTED;
            }
            animations[an->get_id()] = an;
        }
        pending_animations.clear();
    }
}
