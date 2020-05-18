#include "cr-update-functions-manager.hpp"
#include "asset/cr-asset-manager.hpp"
#include "cr-static.hpp"
#include "sync/cr-sync-kernel-workers.hpp"
#include <utility>

void gearoenix::core::UpdateFunctionsManager::update_kernel(const unsigned int kernel_index) noexcept
{
    unsigned int task_number = 0;
    const unsigned int kernels_count = workers->get_threads_count();
    for (const auto& id_fun : update_functions) {
        if (task_number == kernel_index) {
            id_fun.second();
        }
        task_number = (task_number + 1) % kernels_count;
    }
}

gearoenix::core::UpdateFunctionsManager::UpdateFunctionsManager(sync::KernelWorkers* const workers) noexcept
    : workers(workers)
{
    workers->add_step([this](const unsigned int ki) { update_kernel(ki); });
}

gearoenix::core::UpdateFunctionsManager::~UpdateFunctionsManager() noexcept
{
    GX_GUARD_LOCK(update_functions)
    update_functions.clear();
}

gearoenix::core::Id gearoenix::core::UpdateFunctionsManager::add(std::function<void()> fun) noexcept
{
    GX_GUARD_LOCK(update_functions)
    const auto id = asset::Manager::create_id();
    update_functions[id] = std::move(fun);
    return id;
}

void gearoenix::core::UpdateFunctionsManager::remove(const Id id) noexcept
{
    GX_GUARD_LOCK(update_functions)
    update_functions.erase(id);
}
