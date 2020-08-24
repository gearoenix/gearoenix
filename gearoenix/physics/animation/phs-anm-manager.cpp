#include "phs-anm-manager.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"
#include "phs-anm-animation.hpp"

gearoenix::physics::animation::Manager::Manager(core::sync::KernelWorkers* const kernel_workers) noexcept
    : kernels(static_cast<std::size_t>(kernel_workers->get_threads_count()))
{
    kernel_workers->add_step(
        [this]() noexcept {
            auto now = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration<double>(now - time).count();
            time = now;
#ifndef GX_THREAD_NOT_SUPPORTED
            animations_lock.lock();
#endif
        },
        [this](const unsigned int kernel_index) noexcept {
            const auto kernels_count = static_cast<unsigned int>(kernels.size());
            GX_START_MULTITHREADED_TASKS
            auto& kda = kernels[kernel_index].deleted_animations;
            kda.clear();
            for (const auto& aa : animations) {
                GX_DO_MULTITHREADED_TASK(
                    for (const auto& a
                         : aa.second) {
                        if (a.second->apply(time, duration)) {
                            kda.emplace_back(aa.first, a.first);
                        }
                    })
            }
        },
        []() {},
        [this]() noexcept {
            for (auto& k : kernels) {
                for (const auto& a : k.deleted_animations) {
                    animations[a.first].erase(a.second);
                }
            }
#ifndef GX_THREAD_NOT_SUPPORTED
            animations_lock.unlock();
#endif
        });
}

gearoenix::physics::animation::Manager::~Manager() noexcept
{
    animations.clear();
    kernels.clear();
}

void gearoenix::physics::animation::Manager::add(const core::Id animated_object_id, const std::shared_ptr<Animation>& a) noexcept
{
    GX_GUARD_LOCK(animations)
    animations[animated_object_id].emplace(a->get_id(), a);
}
