#include "phs-engine.hpp"
#include "../core/asset/cr-asset-manager.hpp"
#include "../core/sync/cr-sync-kernel-workers.hpp"
#include "../core/sync/cr-sync-queued-semaphore.hpp"
#include "../core/sync/cr-sync-stop-point.hpp"
#include "../render/engine/rnd-eng-engine.hpp"
#include "../render/scene/rnd-scn-manager.hpp"
#include "../system/sys-app.hpp"
#include "../system/sys-log.hpp"
#include "animation/phs-anm-animation.hpp"
#include <functional>

void gearoenix::physics::Engine::update_uniform_buffers_kernel(const unsigned int kernel_index)
{

}

void gearoenix::physics::Engine::update_uniform_buffers()
{
}

gearoenix::physics::Engine::Engine(const std::shared_ptr<render::engine::Engine>& e)
    : e(e)
	, kernels(new core::sync::KernelWorker())
{
	kernels->add_step(std::bind(&Engine::update_uniform_buffers_kernel, this, std::placeholders::_1), std::bind(&Engine::update_uniform_buffers, this));
}

gearoenix::physics::Engine::~Engine()
{
}

void gearoenix::physics::Engine::add_animation(const std::shared_ptr<animation::Animation> &a)
{
    std::lock_guard<std::mutex> _lg(added_animations_locker);
#ifdef GX_DEBUG_MODE
    if (animations.find(a->get_id()) != animations.end())
        GXUNEXPECTED;
#endif
    added_animations.push_back(a);
}

void gearoenix::physics::Engine::remove_animation(const std::shared_ptr<animation::Animation>& a)
{
	remove_animation(a->get_id());
}

void gearoenix::physics::Engine::remove_animation(core::Id a)
{
	std::lock_guard<std::mutex> _lg(removed_animations_locker);
#ifdef GX_DEBUG_MODE
	if (animations.find(a) == animations.end())
		GXUNEXPECTED;
#endif
	removed_animations.push_back(a);
}

void gearoenix::physics::Engine::update()
{

}