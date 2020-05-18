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
            unsigned int task_number = 0;
            const auto kernels_count = static_cast<unsigned int>(kernels.size());
#define GX_DO_TASK(expr)                                 \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }
            auto& kda = kernels[kernel_index].deleted_animations;
            kda.clear();
            for (const auto& a : animations) {
                GX_DO_TASK(
                    if (a->apply(time, duration)) {
                        kda.push_back(a);
                    })
            }
        },
        []() {},
        [this]() noexcept {
            for (const auto& k : kernels) {
                for (const auto& a : k.deleted_animations) {
                    animations.erase(a);
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

void gearoenix::physics::animation::Manager::add(const std::shared_ptr<Animation>& a) noexcept
{
    GX_GUARD_LOCK(animations)
    animations.insert(a);
}
